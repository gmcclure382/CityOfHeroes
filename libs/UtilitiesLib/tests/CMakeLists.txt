add_executable(UtilitiesLibTests main.c allocation.cpp)
set_target_properties(UtilitiesLibTests PROPERTIES
	C_STANDARD 11 C_STANDARD_REQUIRED ON
	CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(UtilitiesLibTests PRIVATE
	"$<LINK_LIBRARY:WHOLE_ARCHIVE,UtilitiesLib>")
if(MINGW)
	target_link_options(UtilitiesLibTests PRIVATE -static)
endif()
add_test(NAME UtilitiesLibIntegration COMMAND UtilitiesLibTests)
set_tests_properties(UtilitiesLibIntegration PROPERTIES TIMEOUT 120)
