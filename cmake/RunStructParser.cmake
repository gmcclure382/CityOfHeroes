# Required STRUCTPARSER_* inputs are supplied by StructParse(). INPUT_FILE is
# a newline-delimited source manifest, passed verbatim to the parser's stdin.
execute_process(
	COMMAND "${STRUCTPARSER_EXE}"
		"TargetName=${STRUCTPARSER_TARGET_NAME}"
		"SrcDir=${STRUCTPARSER_SRC_DIR}"
		"CommonDir=${STRUCTPARSER_COMMON_DIR}"
		"OutDir=${STRUCTPARSER_OUT_DIR}"
		"IntDir=${STRUCTPARSER_INT_DIR}"
		"Configuration=${STRUCTPARSER_CONFIGURATION}"
		"ProjectDir=${STRUCTPARSER_PROJECT_DIR}"
		"IsExecutable=${STRUCTPARSER_IS_EXECUTABLE}"
	INPUT_FILE "${STRUCTPARSER_INPUT_FILE}"
	RESULT_VARIABLE _structparser_result
)

if(NOT "${_structparser_result}" STREQUAL "0")
	message(FATAL_ERROR "StructParser failed: ${_structparser_result}")
endif()
