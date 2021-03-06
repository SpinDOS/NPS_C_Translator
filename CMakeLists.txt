cmake_minimum_required(VERSION 3.6)
project(NPS_C_Translator)

set(CMAKE_CXX_STANDARD 11)

set(NPS_library_sources
        NPS_library/collection_containers/hash.h
        NPS_library/collection_containers/hash.cpp
        NPS_library/collection_containers/list.h
        NPS_library/collection_containers/list.cpp
        NPS_library/collection_containers/list_adv.h
        NPS_library/collection_containers/THashTable.h
        NPS_library/collection_containers/THashTable.tpp
        NPS_library/collection_containers/TSimpleLinkedList.h
        NPS_library/collection_containers/TSimpleLinkedList.tpp
        NPS_library/heap/heap.h
        NPS_library/heap/heap.cpp
        NPS_library/utils/string_utils.h
        NPS_library/utils/string_utils.cpp
        NPS_library/collection_containers/KeyValuePair.h
        NPS_library/TinyXmlLibrary/tinystr.h
        NPS_library/TinyXmlLibrary/tinystr.cpp
        NPS_library/TinyXmlLibrary/tinyxml.h
        NPS_library/TinyXmlLibrary/tinyxml.cpp
        NPS_library/TinyXmlLibrary/tinyxmlerror.cpp
        NPS_library/TinyXmlLibrary/tinyxmlparser.cpp
        NPS_library/collection_containers/KeyValuePair.h
        NPS_library/InterpreterTNodeType.h)

set(NPS_Compiler_sources
        NPS_Compiler/ErrorReporter/ErrorReporter.h
        NPS_Compiler/ErrorReporter/ErrorReporter.cpp
        NPS_Compiler/LexemeParsing/LexemeParser.h
        NPS_Compiler/LexemeParsing/LexemeParser.cpp
        NPS_Compiler/LexemeParsing/LexemeParseStatesManager.h
        NPS_Compiler/LexemeParsing/LexemeParseStatesManager.cpp
        NPS_Compiler/LexemeParsing/LexemeParsingInstructions.ini
        NPS_Compiler/LexemeParsing/LexemePatternCases.h
        NPS_Compiler/Operations/PrimitiveOperationsManager.h
        NPS_Compiler/Operations/PrimitiveOperationsManager.cpp
        NPS_Compiler/SourceCodeParsing/SourceCodeParser.h
        NPS_Compiler/SourceCodeParsing/SourceCodeParser.cpp
        NPS_Compiler/TNodes/ResultType.h
        NPS_Compiler/TNodes/TNode.h
        NPS_Compiler/TNodes/TNode.cpp
        NPS_Compiler/Types/TypeInfo.h
        NPS_Compiler/Types/TypesManager.h
        NPS_Compiler/Types/TypesManager.cpp
        NPS_Compiler/Variables/VariableTable.h
        NPS_Compiler/Variables/VariableTable.cpp
        NPS_Compiler/main.cpp
        NPS_Compiler/TypeCast/TypeCastManager.cpp
        NPS_Compiler/TypeCast/TypeCastManager.h
        NPS_Compiler/SourceCodeParsing/KeywordHandlers.cpp
        NPS_Compiler/SourceCodeParsing/SentenceParsing.cpp 
        NPS_Compiler/TNodes/GetTNode.cpp 
        NPS_Compiler/Operations/OperationsManager.cpp 
        NPS_Compiler/Operations/OperationsManager.h
        NPS_Compiler/TNodes/TNodeSerializaion.cpp)

set(NPS_Interpreter_sources
        NPS_Interpreter/Deserialization/TreeParser.h
        NPS_Interpreter/Deserialization/TreeParser.cpp
        NPS_Interpreter/Operations/OperationManager.h
        NPS_Interpreter/Operations/OperationManager.cpp
        NPS_Interpreter/Operations/KeywordManager.cpp
        NPS_Interpreter/TNodes/TNode.h
        NPS_Interpreter/TNodes/TNode.cpp
        NPS_Interpreter/Variables/VariableTable.h
        NPS_Interpreter/Variables/VariableTable.cpp
        NPS_Interpreter/main.cpp
        NPS_Interpreter/Operations/SystemFunctions.cpp)

add_library(NPS_library SHARED ${NPS_library_sources})
add_executable(NPS_Compiler ${NPS_Compiler_sources})
add_executable(NPS_Interpreter ${NPS_Interpreter_sources})


target_link_libraries(NPS_Compiler NPS_library)
target_link_libraries(NPS_Interpreter NPS_library)
