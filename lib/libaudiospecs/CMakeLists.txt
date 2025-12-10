cmake_minimum_required(VERSION 3.16)

project(libaudiospecs VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PRIVATE src/AudioBuffer.c
  PUBLIC FILE_SET
         myHeaders
         TYPE
         HEADERS
         BASE_DIRS
         include
         FILES
         include/libaudiospecs.h)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic
                                               -fdiagnostics-color=always)
