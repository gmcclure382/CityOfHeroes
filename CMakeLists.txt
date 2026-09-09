cmake_minimum_required(VERSION 3.24...3.31)

project(CoX LANGUAGES C CXX)

set(BUILD_TESTING OFF)
set(BUILD_SHARED_LIBS OFF)
option(COX_BUILD_UTILITIES_TESTS "Build UtilitiesLib integration checks" OFF)
if(COX_BUILD_UTILITIES_TESTS)
	enable_testing()
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

add_subdirectory(Utilities/StructParser)

if(WIN32)
	if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
		message(FATAL_ERROR "CoX supports only 32-bit Windows builds")
	endif()
	include(dependencies/Zlib)
	include(dependencies/CryptoPP)
	add_subdirectory(libs/crashrpt)
	add_subdirectory(libs/UtilitiesLib)
endif()
