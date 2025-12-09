cmake_minimum_required(VERSION 3.16)

project(cPlayer VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fdiagnostics-color=always -g3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

include(CTest)
enable_testing() # needs to be done before building libs to build lib tests

include(cmake/fetch_portaudio.cmake)
set(FILE_READER_BUFFER_SIZE
    16384
    CACHE STRING "" FORCE)
add_subdirectory(lib)
add_subdirectory(src)

include(cmake/fetch_gtest.cmake)
add_subdirectory(test)

add_test(NAME OptionsTest COMMAND OptionsTest)
add_test(NAME WavTest COMMAND WavTest)

set_tests_properties(WavTest PROPERTIES DEPENDS bitcastTest)
