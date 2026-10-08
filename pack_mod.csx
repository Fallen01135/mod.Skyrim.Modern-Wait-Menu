// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).
//
// Note: This code was created with the assistance of AI (Gemini, Claude)
// and was revised and fixed by me.

// Generic mod packer. Everything mod-specific lives in pack_mod_config.json.
// Usage: dotnet script pack_mod.csx -- [repoDir] [configFile] [--no-pause]

using System.IO;
using System.Linq;
using System.IO.Compression;
using System.Text.Json;

// The window stays open at the end unless --no-pause is given (or input is redirected, e.g. CI).
bool pause = !Args.Contains("--no-pause") && !Console.IsInputRedirected;
var positional = Args.Where(a => !a.StartsWith("--")).ToList();

string repoDir = Path.GetFullPath(positional.Count > 0 ? positional[0] : Environment.CurrentDirectory);
string configPath = positional.Count > 1 ? positional[1] : Path.Combine(repoDir, "pack_mod_config.json");

if (!File.Exists(configPath))
{
	Console.Error.WriteLine($"Config not found: {configPath}");
	WaitForKey();
	return;
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
	return;
}

string outputDir = Path.GetFullPath(Path.Combine(repoDir,
	root.TryGetProperty("outputDir", out var outProp) ? outProp.GetString() : "dist"));
string zipPath = Path.Combine(outputDir, zipName);
string tempDir = Path.Combine(repoDir, "_temp_pack");

int missing = 0;

if (Directory.Exists(tempDir))
	Directory.Delete(tempDir, true);
if (File.Exists(zipPath))
	File.Delete(zipPath);

Directory.CreateDirectory(tempDir);
Directory.CreateDirectory(outputDir);

try
{
	// Items from other locations (e.g. a shared library), paths relative to the repo.
	if (root.TryGetProperty("external", out var externals))
	{
		foreach (var ext in externals.EnumerateArray())
		{
			string extRoot = Path.GetFullPath(Path.Combine(repoDir, ext.GetProperty("root").GetString()));
			foreach (var item in ext.GetProperty("include").EnumerateArray())
				CopyItem(extRoot, item.GetString());
		}
	}

	// Items of the mod itself.
	foreach (var item in root.GetProperty("include").EnumerateArray())
		CopyItem(repoDir, item.GetString());

	if (missing > 0)
		Console.Error.WriteLine($"Warning: {missing} item(s) from the config were not found.");

	ZipFile.CreateFromDirectory(tempDir, zipPath);
	Console.WriteLine($"Packed {zipName}");
}
catch (Exception ex)
{
	Console.Error.WriteLine($"Packing failed: {ex}");
}
finally
{
	if (Directory.Exists(tempDir))
		Directory.Delete(tempDir, true);
}

WaitForKey();

void WaitForKey()
{
	if (!pause)
		return;

	Console.WriteLine();
	Console.WriteLine("Press any key to close...");
	Console.ReadKey(true);
}

void CopyItem(string baseDir, string item)
{
	string source = Path.Combine(baseDir, item);
	string dest = Path.Combine(tempDir, item);

	if (Directory.Exists(source))
		CopyDirectory(source, dest);
	else if (File.Exists(source))
	{
		Directory.CreateDirectory(Path.GetDirectoryName(dest));
		File.Copy(source, dest, true);
	}
	else
	{
		Console.Error.WriteLine($"Not found: {source}");
		missing++;
	}
}

void CopyDirectory(string sourceDir, string destinationDir)
{
	Directory.CreateDirectory(destinationDir);

	foreach (string file in Directory.GetFiles(sourceDir))
		File.Copy(file, Path.Combine(destinationDir, Path.GetFileName(file)), true);

	foreach (string subDir in Directory.GetDirectories(sourceDir))
		CopyDirectory(subDir, Path.Combine(destinationDir, new DirectoryInfo(subDir).Name));
}
