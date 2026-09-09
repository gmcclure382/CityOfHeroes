project(StructParser)

set(STRUCTPARSER_SOURCES
    src/AutoRunManager.cpp
    src/AutoTestManager.cpp
    src/AutoTransactionManager.cpp
    src/FileListLoader.cpp
    src/FileListWriter.cpp
    src/FileWrapper.cpp
    src/IdentifierDictionary.cpp
    src/LateLinkManager.cpp
    src/MagicCommandManager.cpp
    src/main.cpp
    src/pch.cpp
    src/SourceParser.cpp
    src/SourceParserBaseClass.cpp
    src/StructParser.cpp
    src/strutils.cpp
    src/Tokenizer.cpp
    src/utils.cpp
    src/version.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${STRUCTPARSER_SOURCES})

set(STRUCTPARSER_HEADERS
    src/AutoRunManager.h
    src/AutoTestManager.h
    src/AutoTransactionManager.h
    src/FileListLoader.h
    src/FileListWriter.h
    src/FileWrapper.h
    src/IdentifierDictionary.h
    src/LateLinkManager.h
    src/MagicCommandManager.h
    src/pch.h
    src/SourceParser.h
    src/SourceParserBaseClass.h
    src/structparser.h
    src/strutils.h
    src/tokenizer.h
    src/utils.h
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${STRUCTPARSER_HEADERS})

add_executable(StructParser ${STRUCTPARSER_SOURCES} ${STRUCTPARSER_HEADERS})
target_compile_features(StructParser PRIVATE cxx_std_17)

set_target_properties(StructParser
    PROPERTIES
    FOLDER "Utilities"
)

if(WIN32)
    target_link_libraries(StructParser PRIVATE shlwapi)
endif()

target_compile_definitions(StructParser
    PRIVATE
    UNICODE=1
    _UNICODE=1
)

if(MSVC)
    target_compile_definitions(StructParser PRIVATE _CRT_SECURE_NO_WARNINGS=1)
    target_compile_options(StructParser PRIVATE /Zc:preprocessor /Zc:__cplusplus)
elseif(MINGW)
    target_link_options(StructParser PRIVATE -static-libstdc++ -static-libgcc)
endif()
