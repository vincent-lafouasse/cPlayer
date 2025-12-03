cmake_minimum_required(VERSION 3.16)

project(bitcast VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fdiagnostics-color=always -g3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
PRIVATE
	src/bitcast.c

PUBLIC
	FILE_SET myHeaders
	TYPE HEADERS
	BASE_DIRS
		include
	FILES
		include/bitcast.h
)

add_subdirectory(test)
include(CTest)
enable_testing()

add_test(NAME bitcastTest
    COMMAND bitcastTest
)
