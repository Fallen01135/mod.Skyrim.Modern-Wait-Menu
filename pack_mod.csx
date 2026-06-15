using System.IO;
using System.IO.Compression;


string zipName = "Modern Wait Menu.zip";

string[] includeFromLib =
{
	"Interface/Shared Elements/PlatformButtons.swf"
};

string[] includeItems =
{
	// Folders
	"Interface",
	"SKSE",

	// Files
	"ModernWaitMenu.esl"
};

string currentDir = (Args.Count > 0)
	? Args[0]
	: Environment.CurrentDirectory;

string AS2LibraryPath = Path.GetFullPath(Path.Combine(currentDir, "..", "AS2 Library"));

string tempDir = Path.Combine(currentDir, "_temp_pack");


if (File.Exists(Path.Combine(currentDir, zipName))) 
	File.Delete(Path.Combine(currentDir, zipName));

Directory.CreateDirectory(tempDir);

foreach (var item in includeFromLib)
{
	string source = Path.Combine(AS2LibraryPath, item);
	string dest = Path.Combine(tempDir, item);

	if (Directory.Exists(source))
		CopyDirectory(source, dest);
	else if (File.Exists(source))
	{
		Directory.CreateDirectory(Path.GetDirectoryName(dest));
		File.Copy(source, dest);
	}
}

foreach (var item in includeItems)
{
	string source = Path.Combine(currentDir, item);
	string dest = Path.Combine(tempDir, item);

	if (Directory.Exists(source))
		CopyDirectory(source, dest);
	else if (File.Exists(source))
	{
		Directory.CreateDirectory(Path.GetDirectoryName(dest));
		File.Copy(source, dest);
	}
}

ZipFile.CreateFromDirectory(tempDir, Path.Combine(currentDir, zipName));
Directory.Delete(tempDir, true);

Console.WriteLine("Mod packed!");

void CopyDirectory(string sourceDir, string destinationDir)
{
	Directory.CreateDirectory(destinationDir);

	foreach (string file in Directory.GetFiles(sourceDir))
		File.Copy(file, Path.Combine(destinationDir, Path.GetFileName(file)), true);

	foreach (string subDir in Directory.GetDirectories(sourceDir))
	{
		string subDirName = new DirectoryInfo(subDir).Name;
		CopyDirectory(subDir, Path.Combine(destinationDir, subDirName));
	}
}