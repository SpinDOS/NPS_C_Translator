cmake_minimum_required(VERSION 3.6)
project(NPS_C_Translator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        hash.h
        hash.cpp
        heap.cpp
        heap.h
        list.cpp
        list.h
        list_adv.h
        main.cpp
        README.md
        string_utils.cpp
        string_utils.h)

add_executable(NPS_C_Translator ${SOURCE_FILES})