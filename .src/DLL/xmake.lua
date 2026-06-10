-- Using https://github.com/CharmedBaryon/CommonLibSSE-NG
includes("lib/CommonLibSSE-NG-main")

set_project("ModernWaitMenu")
set_version("1.0.0")
set_license("Proprietary")

set_languages("c++23")
set_warnings("allextra")

add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

add_requires("boost", {configs = {filesystem = true, fstream = true}})
add_requires("nlohmann_json")

target("ModernWaitMenu")
	add_rules("commonlibsse-ng.plugin",
	{
		name = "Modern Wait Menu",
		author = "Fallen011[35]",
		description = "UI Component for Skyblivion"
	})

	add_deps("commonlibsse-ng")

	add_packages("boost", "nlohmann_json")

	add_files("src/**.cpp")

	add_headerfiles("src/**.hpp")
	add_headerfiles("include/**.hpp")

	add_includedirs("src")
	add_includedirs("include")

	set_pcxxheader("src/pch.hpp")

	after_build(function (target)
		import("core.project.project")

		local output_folder = path.join(target:scriptdir(), "..", "SKSE", "Plugins")
		os.mkdir(output_folder)

		local target_file = target:targetfile()
		os.cp(target_file, output_folder)
		cprint("${green}[SKSE Deploy] DLL copied to: %s", output_folder)

		if is_mode("debug") then
			local pdb_file = path.join(path.directory(target_file), target:basename() .. ".pdb")
			if os.isfile(pdb_file) then
				os.cp(pdb_file, output_folder)
				cprint("${green}[SKSE Deploy] PDB Symbols copied.")
			end
		end
	end)