cmake_minimum_required(VERSION 3.6)
project(NPS_C_Translator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        collection_containers/hash.cpp
        collection_containers/hash.h
        heap/heap.cpp
        heap/heap.h
        collection_containers/list.cpp
        collection_containers/list.h
        collection_containers/list_adv.h
        main.cpp
        utils/string_utils.cpp
        utils/string_utils.h
        LexemeParsing/LexemeParser.h
        LexemeParsing/LexemeParser.cpp
        LexemeParsing/LexemeType.h
        LexemeParsing/LexemeInfo.h
        LexemeParsing/LexemeError.h LexemeParsing/ConstantLexemeParser.cpp)

add_executable(NPS_C_Translator ${SOURCE_FILES})