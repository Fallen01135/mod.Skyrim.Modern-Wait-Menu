cmake_minimum_required(VERSION 3.24)

project(
	Modern-Wait-Menu
	VERSION 2.1.2
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_SKYRIMVR "Build for Skyrim VR" OFF)


get_filename_component(MWM_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

set(MWM_CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/SKSE-CMakeModules/Modules" CACHE PATH
	"Directory containing SKSEPlugin.cmake (SKSE-CMakeModules)")
set(MWM_COMMONLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE" CACHE PATH
	"CommonLibSSE checkout used for the default (SE 1.6+/AE) build")
set(MWM_COMMONLIB_VR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE-NG" CACHE PATH
	"CommonLibSSE-NG checkout used for the SE 1.5.97 / VR build")
set(MWM_DEPLOY_DIR "${MWM_REPO_ROOT}/SKSE/Plugins" CACHE PATH
	"DLL and PDB are copied here after each build. Set to empty to disable.")


function(mwm_require_path path description)
	if (NOT EXISTS "${path}")
		message(FATAL_ERROR
			"${description} not found: ${path}\n"
			"Run update_submodules.bat or set the path via -D.")
	endif()
endfunction()

mwm_require_path("${MWM_CMAKE_MODULES_DIR}/SKSEPlugin.cmake" "SKSE-CMakeModules")


list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${MWM_CMAKE_MODULES_DIR}")

# Platform Switch.
if (BUILD_SKYRIMVR)
	message(STATUS "[Build Target] SKYRIM SE 1.5.97 / VR (CommonLibSSE-NG)")

	mwm_require_path("${MWM_COMMONLIB_VR_DIR}" "CommonLibSSE-NG")
	set(SKSE_COMMONLIBSSE_PATH "${MWM_COMMONLIB_VR_DIR}" CACHE PATH "" FORCE)

	option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE" ON)
	option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE" OFF)
	option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR" ON)
else()
	message(STATUS "[Build Target] SKYRIM SE 1.6+ / AE (CommonLibSSE)")
	mwm_require_path("${MWM_COMMONLIB_DIR}" "CommonLibSSE")

	set(SKSE_COMMONLIBSSE_PATH "${MWM_COMMONLIB_DIR}" CACHE PATH "" FORCE)
	set(SKSE_SUPPORT_VR OFF)
endif()

set(BUILD_TESTS OFF)
set(BUILD_TESTING OFF)

# Stuff
include(SKSEPlugin)

file(GLOB_RECURSE MWM_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

SKSEPlugin_Add(
	${PROJECT_NAME}
	SOURCE_DIR src
	SOURCES
		${MWM_SOURCES}
		vcpkg.json
	PRECOMPILE_HEADERS
		src/PCH.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

if (BUILD_SKYRIMVR)
	target_compile_definitions(${PROJECT_NAME} PRIVATE SKYRIMVR=1)
endif()

# Dependencies / Packages
find_package(spdlog REQUIRED)
find_package(SimpleIni CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(nlohmann_json REQUIRED)
find_package(binary_io REQUIRED)

# SimpleSettings is a header-only library shipped in extern/. Once it lives in
# its own repository, only this block has to change.
add_library(SimpleSettings INTERFACE)
target_include_directories(SimpleSettings INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/extern/SimpleSettings")
target_compile_features(SimpleSettings INTERFACE cxx_std_23)
target_link_libraries(SimpleSettings INTERFACE SimpleIni::SimpleIni)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE
		SimpleSettings
		spdlog::spdlog
		Boost::filesystem
		nlohmann_json::nlohmann_json
		binary_io::binary_io
)

set_target_properties(CommonLibSSE PROPERTIES FOLDER External)


# ----------------------------------------------------------------------------
# Generated settings header (config/settings.tsv -> <build>/generated/Settings.hpp)
# ----------------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(MWM_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
set(MWM_SETTINGS_HEADER "${MWM_GENERATED_DIR}/Settings.hpp")

add_custom_command(
	OUTPUT "${MWM_SETTINGS_HEADER}"
	COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_settings.py"
		--input "${CMAKE_CURRENT_SOURCE_DIR}/config/settings.tsv"
		--output "${MWM_SETTINGS_HEADER}"
		--namespace ModernWaitMenu
		--struct-name Settings
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/config/settings.tsv"
		"${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_settings.py"
	COMMENT "Generating Settings.hpp from config/settings.tsv"
	VERBATIM
)

add_custom_target(mwm_generate_settings DEPENDS "${MWM_SETTINGS_HEADER}")
set_target_properties(mwm_generate_settings PROPERTIES FOLDER Generated)

add_dependencies(${PROJECT_NAME} mwm_generate_settings)
target_include_directories(${PROJECT_NAME} PRIVATE "${MWM_GENERATED_DIR}")


# ----------------------------------------------------------------------------
# Optional deployment
# ----------------------------------------------------------------------------
if (MWM_DEPLOY_DIR)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${MWM_DEPLOY_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:${PROJECT_NAME}>"
			"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
			"${MWM_DEPLOY_DIR}/"
		COMMENT "[Deploy] copying DLL and PDB to ${MWM_DEPLOY_DIR}"
		VERBATIM
	)
endif()

if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*")
endif()