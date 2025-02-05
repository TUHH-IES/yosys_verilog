cmake_minimum_required(VERSION 3.30)
project(yosys_verilog)

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libs/win_flex_bison)
        message(FATAL_ERROR "WinFlexBison not installed properly. Read the README.md file for instructions.")
    endif ()

    LIST(APPEND CMAKE_PROGRAM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/win_flex_bison")
    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/win_flex_bison)
endif ()

find_package(BISON)
find_package(FLEX)
BISON_TARGET(verilog_parser ${CMAKE_CURRENT_SOURCE_DIR}/frontends/verilog/verilog_parser.y ${CMAKE_CURRENT_BINARY_DIR}/frontends/verilog/verilog_parser.tab.cc COMPILE_FLAGS "-d -r all")
FLEX_TARGET(verilog_lexer ${CMAKE_CURRENT_SOURCE_DIR}/frontends/verilog/verilog_lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/frontends/verilog/verilog_lexer.cc)
ADD_FLEX_BISON_DEPENDENCY(verilog_lexer verilog_parser)

file(GLOB_RECURSE yosys_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} kernel/*.cc frontends/*.cc backends/*.cc passes/*.cc)
file(GLOB_RECURSE yosys_passes RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} passes/*.cc backends/*.cc)

add_subdirectory(libs/ezsat)
add_subdirectory(libs/json11)
add_subdirectory(libs/bigint)
add_subdirectory(libs/sha1)

add_library(yosys_verilog STATIC ${yosys_files} ${BISON_verilog_parser_OUTPUTS} ${FLEX_verilog_lexer_OUTPUTS})
target_include_directories(yosys_verilog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(yosys_verilog PUBLIC ezsat json11 bigint sha1)

add_executable(main main.cpp ${yosys_passes})
target_link_libraries(main PUBLIC yosys_verilog)