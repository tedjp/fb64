cmake_minimum_required (VERSION 3.10)

project(fb64)

add_library(fb64 fb64.c fb64.h encode.c decode.c)
set_target_properties(fb64 PROPERTIES PUBLIC_HEADER fb64.h)

add_executable(fb64-example example.c)
target_link_libraries(fb64-example PRIVATE fb64)

add_executable(fb64-test test.c)
target_link_libraries(fb64-test PRIVATE fb64)

add_test(NAME test COMMAND fb64-test)
add_test(NAME example COMMAND fb64-example)

install(
	TARGETS fb64
	PUBLIC_HEADER)
