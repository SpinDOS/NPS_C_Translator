cmake_minimum_required(VERSION 3.6)
project(NPS_C_Translator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        heap.cpp
        heap.h
        list.cpp
        list.h
        list_adv.h
        main.cpp
        README.md)

add_executable(NPS_C_Translator ${SOURCE_FILES})