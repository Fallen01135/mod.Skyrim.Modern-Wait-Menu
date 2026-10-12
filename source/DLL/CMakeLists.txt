cmake_minimum_required(VERSION 3.24)

project(
	Modern-Wait-Menu
	VERSION 2.2.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

get_filename_component(FTA_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)


# ----------------------------------------------------------------------------
# Paths and Submodules
# ----------------------------------------------------------------------------
set(FTA_COMMONLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/CommonLibSSE-NG" CACHE PATH
	"CommonLibSSE-NG checkout used for all targets (SE/AE/VR)" FORCE)

set(FTA_CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/SKSE-CMakeModules/Modules" CACHE PATH
	"Directory containing SKSEPlugin.cmake (SKSE-CMakeModules)")

set(FTA_DEPLOY_DIR "${FTA_REPO_ROOT}/SKSE/Plugins" CACHE PATH
	"DLL and PDB are copied here after each build. Set to empty to disable.")

function(fta_require_path path description)
	if (NOT EXISTS "${path}")
		message(FATAL_ERROR
			"${description} not found: ${path}\n"
			"Run update_submodules.bat or set the path via -D.")
	endif()
endfunction()

fta_require_path("${FTA_COMMONLIB_DIR}" "CommonLibSSE-NG")
fta_require_path("${FTA_CMAKE_MODULES_DIR}/SKSEPlugin.cmake" "SKSE-CMakeModules")

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${FTA_CMAKE_MODULES_DIR}")

set(SKSE_COMMONLIBSSE_PATH "${FTA_COMMONLIB_DIR}" CACHE PATH "" FORCE)

option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE" ON)
option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE" ON)
option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR" ON)

set(BUILD_TESTS OFF)
set(BUILD_TESTING OFF)


# ----------------------------------------------------------------------------
# TARGET CREATION
# ----------------------------------------------------------------------------
include(SKSEPlugin)

file(GLOB_RECURSE FTA_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

SKSEPlugin_Add(
	${PROJECT_NAME}
	SOURCE_DIR src
	SOURCES
		${FTA_SOURCES}
		vcpkg.json
	PRECOMPILE_HEADERS
		src/PCH.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)


# ----------------------------------------------------------------------------
# DEPENDENCIES
# ----------------------------------------------------------------------------
find_package(spdlog REQUIRED)
find_package(SimpleIni CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(nlohmann_json REQUIRED)

# SimpleSettings (Header-Only)
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
)

set_target_properties(CommonLibSSE PROPERTIES FOLDER External)


# ----------------------------------------------------------------------------
# GENERATED SETTINGS HEADER
# ----------------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(FTA_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
set(FTA_SETTINGS_HEADER "${FTA_GENERATED_DIR}/Settings.hpp")

add_custom_command(
	OUTPUT "${FTA_SETTINGS_HEADER}"
	COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_settings.py"
		--input "${CMAKE_CURRENT_SOURCE_DIR}/config/settings.tsv"
		--output "${FTA_SETTINGS_HEADER}"
		--namespace ModernWaitMenu
		--struct-name Settings
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/config/settings.tsv"
		"${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_settings.py"
	COMMENT "Generating Settings.hpp from config/settings.tsv"
	VERBATIM
)

add_custom_target(fta_generate_settings DEPENDS "${FTA_SETTINGS_HEADER}")
set_target_properties(fta_generate_settings PROPERTIES FOLDER Generated)

add_dependencies(${PROJECT_NAME} fta_generate_settings)
target_include_directories(${PROJECT_NAME} PRIVATE "${FTA_GENERATED_DIR}")


# ----------------------------------------------------------------------------
# DEPLOYMENT
# ----------------------------------------------------------------------------
if (FTA_DEPLOY_DIR)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${FTA_DEPLOY_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:${PROJECT_NAME}>"
			"${FTA_DEPLOY_DIR}/"
		COMMENT "[Deploy] copying DLL to ${FTA_DEPLOY_DIR}"
		VERBATIM
	)
endif()

if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*")
endif()