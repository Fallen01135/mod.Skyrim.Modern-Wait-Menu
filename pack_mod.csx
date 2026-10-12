// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).
//
// Note: This code was created with the assistance of AI (Gemini, Claude)
// and was revised and fixed by me.

// Generic mod packer. Everything mod-specific lives in pack_mod_config.json.
// Usage: dotnet script pack_mod.csx -- [repoDir] [configFile] [--no-pause] [--dry-run] [--version=1.2.3]
//
// Config (all keys except zipName and include are optional):
//   zipName   Name of the zip. "{version}" is replaced (see below).
//   version   Only used if zipName contains {version}. Priority: --version=1.2.3, then this key:
//               "1.2.3"                                      fixed version
//               { "file": "vcpkg.json",                      read from a file (relative to repo),
//                 "regex": "\"version\"\\s*:\\s*\"([^\"]+)\"" first capture group = version
//             Without the key: CMakeLists.txt with project(Name VERSION x.y.z ...).
//   outputDir Output folder, relative to the repo (default "dist").
//   include   List of entries. An entry is either a string ("Interface") or an object:
//               { "from": "...", "to": "...", "root": "...", "optional": true }
//               from      File, folder or wildcard pattern (*, **, ?), relative to root.
//               to        Target path inside the zip (default: same as from). Not allowed with wildcards.
//                         Ends with "/" -> target is a folder and the file name is kept.
//               root      Base folder for "from" (default: repo dir).
//               optional  If true, a missing source is only reported, not an error.
//   exclude   List of patterns, matched against the path inside the zip.
//             Without "/" it matches any file or folder name ("*.psd", "Thumbs.db"),
//             with "/" it matches the whole path ("Interface/src", "**/*.fla").
//   external  Old style: [{ "root": "...", "include": [...] }] - still supported.
//   bsa       Optional: pack some of the files into a BSA (see README of the config example).

using System.IO;
using System.Linq;
using System.IO.Compression;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Diagnostics;

// The window stays open at the end unless --no-pause is given (or input is redirected, e.g. CI).
bool pause = !Args.Contains("--no-pause") && !Console.IsInputRedirected;
bool dryRun = Args.Contains("--dry-run");
string versionArg = Args.FirstOrDefault(a => a.StartsWith("--version="))?.Substring("--version=".Length);
var positional = Args.Where(a => !a.StartsWith("--")).ToList();

string repoDir = Path.GetFullPath(positional.Count > 0 ? positional[0] : Environment.CurrentDirectory);
string configPath = positional.Count > 1 ? positional[1] : Path.Combine(repoDir, "pack_mod_config.json");

if (!File.Exists(configPath))
{
	Console.Error.WriteLine($"Config not found: {configPath}");
	WaitForKey();
	return 1;
}

JsonElement root;
string zipName;
try
{
	root = JsonDocument.Parse(File.ReadAllText(configPath)).RootElement;
	zipName = root.GetProperty("zipName").GetString();
}
catch (Exception ex)
{
	Console.Error.WriteLine($"Invalid config {configPath}: {ex.Message}");
	WaitForKey();
	return 1;
}

if (zipName.Contains("{version}"))
{
	string version = versionArg ?? ResolveVersion();

	if (string.IsNullOrWhiteSpace(version))
	{
		Console.Error.WriteLine("zipName contains {version}, but no version was found.");
		Console.Error.WriteLine("Use --version=1.2.3 or set \"version\" in the config (fixed string, or { \"file\": ..., \"regex\": ... }).");
		WaitForKey();
		return 1;
	}

	zipName = zipName.Replace("{version}", version);
}

string outputDir = Path.GetFullPath(Path.Combine(repoDir,
	root.TryGetProperty("outputDir", out var outProp) ? outProp.GetString() : "dist"));
string zipPath = Path.Combine(outputDir, zipName);

// Everything that goes into the zip: path inside the zip -> source file on disk.
var packed = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
var excludeRules = new List<Func<string, bool>>();
int problems = 0;
int excluded = 0;
int exitCode = 0;
string bsaTemp = null;
var timer = Stopwatch.StartNew();

try
{
	if (root.TryGetProperty("exclude", out var excludeProp))
		foreach (var pattern in excludeProp.EnumerateArray())
			excludeRules.Add(MakeExclude(pattern.GetString()));

	// Old style: items from other locations (e.g. a shared library).
	if (root.TryGetProperty("external", out var externals))
	{
		foreach (var ext in externals.EnumerateArray())
		{
			string extRoot = Path.GetFullPath(Path.Combine(repoDir, ext.GetProperty("root").GetString()));
			foreach (var item in ext.GetProperty("include").EnumerateArray())
				AddEntry(item, extRoot);
		}
	}

	// Items of the mod itself.
	foreach (var item in root.GetProperty("include").EnumerateArray())
		AddEntry(item, repoDir);

	if (problems > 0)
	{
		Console.Error.WriteLine($"{problems} problem(s) found - nothing was packed.");
		exitCode = 1;
	}
	else
	{
		if (root.TryGetProperty("bsa", out var bsaCfg))
			BuildBsa(bsaCfg);

		if (dryRun)
		{
			foreach (var kv in packed.OrderBy(k => k.Key, StringComparer.Ordinal))
				Console.WriteLine($"  {kv.Key}   <-   {kv.Value}");

			Console.WriteLine();
			Console.WriteLine($"Dry run: {packed.Count} file(s) would go into {zipName} ({excluded} excluded).");
		}
		else
		{
			Directory.CreateDirectory(outputDir);
			WriteZip();

			double mb = new FileInfo(zipPath).Length / 1048576.0;
			Console.WriteLine($"Packed {zipName}: {packed.Count} file(s), {mb:0.00} MB, {excluded} excluded, {timer.Elapsed.TotalSeconds:0.0}s");
		}
	}
}
catch (Exception ex)
{
	Console.Error.WriteLine($"Packing failed: {ex}");
	exitCode = 1;
}
finally
{
	if (bsaTemp != null && Directory.Exists(bsaTemp))
		Directory.Delete(bsaTemp, true);
}

WaitForKey();
return exitCode;

// ---------------------------------------------------------------------------

void WaitForKey()
{
	if (!pause)
		return;

	Console.WriteLine();
	Console.WriteLine("Press any key to close...");
	Console.ReadKey(true);
}

// "version" in the config is either a fixed string or an object { "file": "...", "regex": "..." }.
// Default: CMakeLists.txt with project(Name VERSION x.y.z ...).
// The regex should have one capture group around the version; without a group the whole match is used.
string ResolveVersion()
{
	string file = "CMakeLists.txt";
	string pattern = @"project\s*\([^)]*?\bVERSION\s+(\d+(?:\.\d+)*)";

	if (root.TryGetProperty("version", out var versionProp))
	{
		if (versionProp.ValueKind == JsonValueKind.String)
			return versionProp.GetString();

		if (versionProp.TryGetProperty("file", out var fileProp))
			file = fileProp.GetString();
		if (versionProp.TryGetProperty("regex", out var regexProp))
			pattern = regexProp.GetString();
	}

	string path = Path.GetFullPath(Path.Combine(repoDir, file));
	if (!File.Exists(path))
	{
		Console.Error.WriteLine($"Version file not found: {path}");
		return null;
	}

	var match = Regex.Match(File.ReadAllText(path), pattern, RegexOptions.IgnoreCase);
	if (!match.Success)
	{
		Console.Error.WriteLine($"Version regex found nothing in {path}");
		return null;
	}

	return match.Groups.Count > 1 ? match.Groups[1].Value : match.Groups[0].Value;
}

// Handles one entry of "include": a string or an object with from/to/root/optional.
void AddEntry(JsonElement entry, string defaultRoot)
{
	string from;
	string to = null;
	string baseDir = defaultRoot;
	bool optional = false;

	if (entry.ValueKind == JsonValueKind.String)
		from = entry.GetString();
	else
	{
		from = entry.GetProperty("from").GetString();
		if (entry.TryGetProperty("to", out var toProp))
			to = toProp.GetString();
		if (entry.TryGetProperty("root", out var rootProp))
			baseDir = Path.GetFullPath(Path.Combine(defaultRoot, rootProp.GetString()));
		if (entry.TryGetProperty("optional", out var optProp))
			optional = optProp.GetBoolean();
	}

	from = from.Replace('\\', '/');
	int wild = from.IndexOfAny(new[] { '*', '?' });

	// Wildcard pattern: keeps the relative path, no "to".
	if (wild >= 0)
	{
		if (to != null)
		{
			Console.Error.WriteLine($"\"to\" is not supported together with wildcards: {from}");
			problems++;
			return;
		}

		// Only search below the folder that comes before the first wildcard.
		int slash = from.LastIndexOf('/', wild);
		string searchDir = Path.Combine(baseDir, slash > 0 ? from.Substring(0, slash) : "");
		var regex = GlobToRegex(from, false);

		var matches = new List<string>();
		if (Directory.Exists(searchDir))
		{
			foreach (var file in Directory.EnumerateFiles(searchDir, "*", SearchOption.AllDirectories))
			{
				string rel = Path.GetRelativePath(baseDir, file).Replace('\\', '/');
				if (regex.IsMatch(rel))
					matches.Add(rel);
			}
		}

		if (matches.Count == 0)
			NotFound(from, optional);

		foreach (var rel in matches)
			AddFile(Path.Combine(baseDir, rel), rel);

		return;
	}

	string source = Path.Combine(baseDir, from);
	string dest = (to ?? from).Replace('\\', '/');

	if (Directory.Exists(source))
	{
		foreach (var file in Directory.EnumerateFiles(source, "*", SearchOption.AllDirectories))
		{
			string rel = Path.GetRelativePath(source, file).Replace('\\', '/');
			AddFile(file, dest.TrimEnd('/') + "/" + rel);
		}
	}
	else if (File.Exists(source))
	{
		if (dest.EndsWith("/"))
			dest += Path.GetFileName(source);
		AddFile(source, dest);
	}
	else
		NotFound(source, optional);
}

void NotFound(string what, bool optional)
{
	if (optional)
		Console.WriteLine($"Skipped (optional, not found): {what}");
	else
	{
		Console.Error.WriteLine($"Not found: {what}");
		problems++;
	}
}

void AddFile(string source, string destRel)
{
	destRel = destRel.Replace('\\', '/').TrimStart('/');

	if (destRel.Length == 0 || Path.IsPathRooted(destRel) || destRel.Split('/').Contains(".."))
	{
		Console.Error.WriteLine($"Invalid target path: {destRel}");
		problems++;
		return;
	}

	if (excludeRules.Any(rule => rule(destRel)))
	{
		excluded++;
		return;
	}

	if (packed.TryGetValue(destRel, out var previous))
		Console.Error.WriteLine($"Warning: {destRel} is added twice, the later one wins ({previous} -> {source})");

	packed[destRel] = source;
}

// "*" = anything except "/", "**" = anything including "/", "?" = one character except "/".
// "**/" also matches zero folders. matchBelow = also match everything below a matching folder.
Regex GlobToRegex(string glob, bool matchBelow)
{
	var sb = new StringBuilder("^");

	for (int i = 0; i < glob.Length; i++)
	{
		char c = glob[i];

		if (c == '*')
		{
			if (i + 1 < glob.Length && glob[i + 1] == '*')
			{
				i++;
				if (i + 1 < glob.Length && glob[i + 1] == '/')
				{
					i++;
					sb.Append("(?:.*/)?");
				}
				else
					sb.Append(".*");
			}
			else
				sb.Append("[^/]*");
		}
		else if (c == '?')
			sb.Append("[^/]");
		else
			sb.Append(Regex.Escape(c.ToString()));
	}

	sb.Append(matchBelow ? "(?:/.*)?$" : "$");
	return new Regex(sb.ToString(), RegexOptions.IgnoreCase);
}

Func<string, bool> MakeExclude(string pattern)
{
	pattern = pattern.Replace('\\', '/').Trim('/');

	// No "/": matches the name of any file or folder in the path (like .gitignore).
	if (!pattern.Contains('/'))
	{
		var segment = GlobToRegex(pattern, false);
		return path => path.Split('/').Any(part => segment.IsMatch(part));
	}

	var full = GlobToRegex(pattern, true);
	return path => full.IsMatch(path);
}

void WriteZip()
{
	string tmpZip = zipPath + ".tmp";

	try
	{
		if (File.Exists(tmpZip))
			File.Delete(tmpZip);

		using (var zip = ZipFile.Open(tmpZip, ZipArchiveMode.Create))
		{
			foreach (var kv in packed.OrderBy(k => k.Key, StringComparer.Ordinal))
				zip.CreateEntryFromFile(kv.Value, kv.Key, CompressionLevel.Optimal);
		}

		File.Move(tmpZip, zipPath, true);
	}
	finally
	{
		if (File.Exists(tmpZip))
			File.Delete(tmpZip);
	}
}

// Packs the files below the configured folders into a BSA using an external tool
// and swaps them in the package for the finished BSA.
void BuildBsa(JsonElement cfg)
{
	string toolCfg = cfg.GetProperty("tool").GetString();
	string argsTemplate = cfg.GetProperty("args").GetString();
	string bsaName = cfg.GetProperty("name").GetString();
	bool removeLoose = !cfg.TryGetProperty("removeLooseFiles", out var removeProp) || removeProp.GetBoolean();

	var prefixes = cfg.GetProperty("include").EnumerateArray()
		.Select(p => p.GetString().Replace('\\', '/').Trim('/') + "/")
		.ToList();

	var inBsa = packed
		.Where(kv => prefixes.Any(p => kv.Key.StartsWith(p, StringComparison.OrdinalIgnoreCase)))
		.ToList();

	if (inBsa.Count == 0)
	{
		Console.WriteLine("BSA: no matching files, skipped.");
		return;
	}

	if (dryRun)
		packed[bsaName] = "(BSA would be generated)";
	else
	{
		bsaTemp = Path.Combine(Path.GetTempPath(), "pack_mod_" + Guid.NewGuid().ToString("N"));
		string inputDir = Path.Combine(bsaTemp, "input");
		string outputFile = Path.Combine(bsaTemp, bsaName);

		foreach (var kv in inBsa)
		{
			string target = Path.Combine(inputDir, kv.Key);
			Directory.CreateDirectory(Path.GetDirectoryName(target));
			File.Copy(kv.Value, target, true);
		}

		// "tool" may be relative to the repo, absolute, or a name found via PATH.
		string tool = Path.GetFullPath(Path.Combine(repoDir, toolCfg));
		if (!File.Exists(tool))
			tool = toolCfg;

		var psi = new ProcessStartInfo(tool, argsTemplate.Replace("{input}", inputDir).Replace("{output}", outputFile))
		{
			UseShellExecute = false
		};

		Console.WriteLine($"BSA: packing {inBsa.Count} file(s) into {bsaName} ...");

		using (var process = Process.Start(psi))
		{
			process.WaitForExit();
			if (process.ExitCode != 0)
				throw new Exception($"BSA tool exited with code {process.ExitCode}.");
		}

		if (!File.Exists(outputFile))
			throw new Exception($"BSA tool did not create {outputFile}.");

		packed[bsaName] = outputFile;
	}

	if (removeLoose)
		foreach (var kv in inBsa)
			packed.Remove(kv.Key);
}
