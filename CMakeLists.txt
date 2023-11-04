cmake_minimum_required(VERSION 3.26)
project(c_json_parser C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_library(c_json_parser parser.c)
add_library(c_json_query query.c)

add_executable(test_parser parser_test.c)
target_link_libraries(test_parser c_json_parser)
add_test(NAME test_parser COMMAND test_parser)

add_executable(test_query query_test.c)
target_link_libraries(test_query c_json_query)
add_test(NAME test_query COMMAND test_query)
