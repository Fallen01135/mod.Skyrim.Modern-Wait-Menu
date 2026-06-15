-- ==================================
-- Project Settings
-- ==================================
local AUTHOR_NAME = "Fallen011[35]"
local VERSION = "1.0.0"
local LICENSE = "Proprietary"

local PROJECT_NAME = "Modern-Wait-Menu"
local PLUGIN_NAME = "Modern-Wait-Menu"
local PLUGIN_DESCRIPTION = "The DLL for the Modern Wait Menu mod"

local OUTPUT_PATH = "../../SKSE/Plugins"

-- Project Structure
local SOURCE_FILES =
{
	"src/**.cpp"
}

local HEADER_FILES =
{
	"src/**.hpp",
	"include/**.hpp"
}

local INCLUDE_DIRS =
{
	"src",
	"include"
}

local PCH_HEADER = "src/PCH.hpp"

-- Frameworks
local INCLUDE_FRAMEWORK_PATHS =
{
	-- Using https://github.com/CharmedBaryon/CommonLibSSE-NG
	"../../.modules/CommonLibSSE-NG"
}

local REQUIRED_FRAMEWORKS =
{
	{
		dep  = "commonlibsse-ng", 
		rule = "commonlibsse-ng.plugin" 
	}
}

-- Packages
local REQUIRED_PACKAGES =
{
	{name = "boost", configs = {filesystem = true, fstream = true}},
	"nlohmann_json",
	"simpleini"
}


-- ==================================
-- Helper Functions
-- ==================================
local function is_absolute_path(p)
	if p:match("^%a+:") or p:match("^\\\\") or p:match("^/") then
		return true
	end

	return false
end

-- ==================================
-- Code Start (No need to change anything below)
-- ==================================
add_rules("mode.debug", "mode.release", "mode.releasedbg")

if INCLUDE_FRAMEWORK_PATHS then
	for _, framework_path in ipairs(INCLUDE_FRAMEWORK_PATHS) do
		includes(framework_path)
	end
end

if REQUIRED_PACKAGES then
	for _, item in ipairs(REQUIRED_PACKAGES) do
		if type(item) == "table" then
			add_requires(item.name, {configs = item.configs})
		else
			add_requires(item)
		end
	end
end

-- Project Definition
set_project(PROJECT_NAME)
set_version(VERSION)
set_license(LICENSE)
set_languages("c++23")
set_warnings("allextra")
add_rules("plugin.vsxmake.autoupdate")

target(PROJECT_NAME)
	if REQUIRED_FRAMEWORKS then
		for _, framework in ipairs(REQUIRED_FRAMEWORKS) do
			if framework.rule then
				add_rules(framework.rule,
				{
					author = AUTHOR_NAME,
					name = PLUGIN_NAME,
					description = PLUGIN_DESCRIPTION
				})
			end
		end

		for _, framework in ipairs(REQUIRED_FRAMEWORKS) do
			add_deps(framework.dep)
		end
	end

	if REQUIRED_PACKAGES then
		for _, item in ipairs(REQUIRED_PACKAGES) do
			if type(item) == "table" then
				add_packages(item.name)
			else
				add_packages(item)
			end
		end
	end

	-- Project structure
	if SOURCE_FILES then
		for _, pattern in ipairs(SOURCE_FILES) do
			add_files(pattern)
		end
	end

	if HEADER_FILES then
		for _, pattern in ipairs(HEADER_FILES) do
			add_headerfiles(pattern)
		end
	end

	if INCLUDE_DIRS then
		for _, dir in ipairs(INCLUDE_DIRS) do
			add_includedirs(dir)
		end
	end

	if PCH_HEADER and PCH_HEADER ~= "" then
		set_pcxxheader(PCH_HEADER)
	end

	-- Automatic DLL Deployment after build
	after_build(function (target)
		import("core.project.project")

		local output_folder
		if is_absolute_path(OUTPUT_PATH) then
			output_folder = OUTPUT_PATH
		else
			output_folder = path.join(target:scriptdir(), OUTPUT_PATH)
		end

		os.mkdir(output_folder)

		local target_file = target:targetfile()
		os.cp(target_file, output_folder)
		cprint("${green}[SKSE Deploy] DLL copied to: %s", output_folder)

		if is_mode("debug") or is_mode("releasedbg") then
			local pdb_file = path.join(path.directory(target_file), target:basename() .. ".pdb")

			if os.isfile(pdb_file) then
				os.cp(pdb_file, output_folder)
				cprint("${green}[SKSE Deploy] PDB Symbols copied.")
			end
		end
	end)