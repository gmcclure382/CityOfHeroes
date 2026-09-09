cmake_minimum_required(VERSION 3.24)
project(CoXDependencyChecks LANGUAGES C CXX)

if(NOT WIN32)
	message(FATAL_ERROR "These checks exercise the Windows dependencies")
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/..")
include(dependencies/Zlib)
include(dependencies/CryptoPP)

add_executable(zlib_check zlib.c)
target_compile_features(zlib_check PRIVATE c_std_11)
target_link_libraries(zlib_check PRIVATE ZLIB::ZLIB)
add_executable(cryptopp_check cryptopp.cpp)
target_compile_features(cryptopp_check PRIVATE cxx_std_11)
target_link_libraries(cryptopp_check PRIVATE cryptopp::cryptopp)
configure_file("${cryptopp_SOURCE_DIR}/TestData/rsa1024.dat"
	"${CMAKE_CURRENT_BINARY_DIR}/rsa1024.dat" COPYONLY)

# Keep MinGW smoke executables runnable without compiler runtime DLL copies.
if(MINGW)
	target_link_options(cryptopp_check PRIVATE -static)
endif()
