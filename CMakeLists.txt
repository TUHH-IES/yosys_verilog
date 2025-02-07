cmake_minimum_required(VERSION 3.30)
set(yosys-verilog_VERSION 0.49.21)
project(yosys-verilog
        VERSION ${yosys-verilog_VERSION}
        DESCRIPTION "A trimmed down version of Yosys (https://github.com/YosysHQ/yosys) easily usable in CMake projects."
        HOMEPAGE_URL "https://github.com/gian21391/yosys_verilog"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FetchContent)

find_program(BISON "bison")

#if(WIN32)
#    FetchContent_Declare(win_flex_bison
#            URL https://github.com/lexxmark/winflexbison/releases/download/v2.5.25/win_flex_bison-2.5.25.zip
#            URL_MD5 720226b1befe7033fb3ecc98f5ffd425
#            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
#    )
#    FetchContent_MakeAvailable(win_flex_bison)
#    LIST(APPEND CMAKE_PROGRAM_PATH "${win_flex_bison_SOURCE_DIR}")
#endif()

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

add_library(yosys-verilog STATIC ${yosys_files} ${BISON_verilog_parser_OUTPUTS} ${FLEX_verilog_lexer_OUTPUTS})
target_include_directories(yosys-verilog PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(yosys-verilog PUBLIC ezsat json11 bigint sha1)

add_executable(main main.cpp ${yosys_passes})
target_link_libraries(main PUBLIC yosys-verilog)

configure_package_config_file(cmake/config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/yosys-verilog-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/yosys-verilog
        NO_SET_AND_CHECK_MACRO)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/yosys-verilog-config-version.cmake
        VERSION ${yosys-verilog_VERSION}
        COMPATIBILITY SameMajorVersion)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/yosys-verilog-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/yosys-verilog-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_DATADIR}/yosys-verilog)
install(TARGETS yosys-verilog ezsat json11 bigint sha1 EXPORT yosys-verilog-targets)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "backends/json" EXCLUDE
        PATTERN "libs/bigint" EXCLUDE)
install(EXPORT yosys-verilog-targets
        NAMESPACE yosys-verilog::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/yosys-verilog)