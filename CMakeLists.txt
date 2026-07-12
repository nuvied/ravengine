cmake_minimum_required(VERSION 3.20)

# Use vcpkg automatically when available and no toolchain is explicitly set.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
endif()

project(ravengine VERSION 0.1.0 LANGUAGES C CXX)

# Basic language standards for a mixed C/C++ project.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
	 add_compile_options(/Zc:preprocessor)
endif()


find_package(spdlog CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(bgfx CONFIG REQUIRED)

if(TARGET bgfx::bgfx)
	set(BGFX_TARGET bgfx::bgfx)
elseif(TARGET unofficial::bgfx::bgfx)
	set(BGFX_TARGET unofficial::bgfx::bgfx)
else()
	message(FATAL_ERROR "bgfx package found, but no supported target was exported")
endif()

add_executable(ravengine
	src/main.cpp
	src/engine.cpp
	src/window.cpp
	src/renderer.cpp
)

target_include_directories(ravengine
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/include/ravengine
)

target_link_libraries(ravengine
	PRIVATE
		SDL2::SDL2
		${BGFX_TARGET}
		spdlog::spdlog
)
