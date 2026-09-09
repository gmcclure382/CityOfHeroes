include_guard()

# StructParse(target) scans a target's sources before compilation. Call after
# assigning its source files. The target's source directory must contain src/;
# Common/ lives at the top of the source tree. Generated files retain their
# existing locations in those directories. StructParser must be runnable on
# the build host. Errors from the parser fail the dependent target's build.
function(StructParse INPUT_TARGET)
	get_target_property(_sources ${INPUT_TARGET} SOURCES)
	get_target_property(_source_dir ${INPUT_TARGET} SOURCE_DIR)
	get_target_property(_binary_dir ${INPUT_TARGET} BINARY_DIR)
	set(_source_list
		"${_binary_dir}/${INPUT_TARGET}.$<CONFIG>.StructParser.sources")
	set(_content "")
	foreach(_source IN LISTS _sources)
		set(_absolute "$<PATH:ABSOLUTE_PATH,${_source},${_source_dir}>")
		string(APPEND _content "$<$<BOOL:${_source}>:${_absolute}>\n")
	endforeach()
	file(GENERATE OUTPUT "${_source_list}" CONTENT "${_content}"
		TARGET ${INPUT_TARGET})

	get_target_property(_type ${INPUT_TARGET} TYPE)
	if(_type STREQUAL "EXECUTABLE")
		set(_is_executable true)
	else()
		set(_is_executable false)
	endif()

	if(CMAKE_CONFIGURATION_TYPES)
		set(_configuration "$<CONFIG>")
	elseif(CMAKE_BUILD_TYPE)
		set(_configuration "${CMAKE_BUILD_TYPE}")
	else()
		set(_configuration "Default")
	endif()
	set(_int_dir "${_binary_dir}/${INPUT_TARGET}.dir/${_configuration}")
	set(_out_dir "$<TARGET_FILE_DIR:${INPUT_TARGET}>")
	set(_runner "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RunStructParser.cmake")

	add_custom_target(${INPUT_TARGET}_StructParser
		COMMAND ${CMAKE_COMMAND}
			"-DSTRUCTPARSER_EXE=$<TARGET_FILE:StructParser>"
			"-DSTRUCTPARSER_TARGET_NAME=${INPUT_TARGET}"
			"-DSTRUCTPARSER_SRC_DIR=${_source_dir}/src"
			"-DSTRUCTPARSER_COMMON_DIR=${CMAKE_SOURCE_DIR}/Common"
			"-DSTRUCTPARSER_OUT_DIR=${_out_dir}"
			"-DSTRUCTPARSER_INT_DIR=${_int_dir}"
			"-DSTRUCTPARSER_CONFIGURATION=${_configuration}"
			"-DSTRUCTPARSER_PROJECT_DIR=${_source_dir}"
			"-DSTRUCTPARSER_IS_EXECUTABLE=${_is_executable}"
			"-DSTRUCTPARSER_INPUT_FILE=${_source_list}"
			-P "${_runner}"
		DEPENDS "${_source_list}"
		COMMENT "Running StructParser on ${INPUT_TARGET}"
		VERBATIM
	)
	add_dependencies(${INPUT_TARGET}_StructParser StructParser)
	add_dependencies(${INPUT_TARGET} ${INPUT_TARGET}_StructParser)
endfunction()
