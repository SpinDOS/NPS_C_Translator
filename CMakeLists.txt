cmake_minimum_required(VERSION 3.6)
project(NPS_C_Translator)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE NPS_library_sources
        "NPS_library/*")

file(GLOB_RECURSE NPS_Compiler_sources
        "NPS_Compiler/*")

add_library(NPS_library SHARED ${NPS_library_sources})
add_executable(NPS_Compiler ${NPS_Compiler_sources})

target_link_libraries(NPS_Compiler NPS_library)
