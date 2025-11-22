cmake_minimum_required(VERSION 3.16)

project(cPlayer VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fdiagnostics-color=always -g3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(FetchContent)

include(cmake/fetch_portaudio.cmake)
add_subdirectory(lib)
add_subdirectory(src)

include(cmake/fetch_gtest.cmake)
add_subdirectory(test)

include(CTest)
enable_testing()

add_test(NAME FileReaderTest
    COMMAND FileReaderTest
)
add_test(NAME bitcastTest
    COMMAND bitcastTest
)
add_test(NAME i24Test
    COMMAND i24Test
)
