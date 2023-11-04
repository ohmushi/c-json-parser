cmake_minimum_required(VERSION 3.26)
project(c_json_parser C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_library(c_json_parser library.c)

add_executable(TESTS test.c)
target_link_libraries(TESTS c_json_parser)
add_test(NAME TESTS COMMAND TESTS)
