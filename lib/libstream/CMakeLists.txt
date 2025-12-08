cmake_minimum_required(VERSION 3.16)

project(libstream VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fdiagnostics-color=always -g3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(NOT TARGET bitcast)
    message(STATUS "Building libstream Standalone: Fetching bitcast dependency.")

    include(FetchContent)

    FetchContent_Declare(
        bitcast_dep
        GIT_REPOSITORY  https://github.com/vincent-lafouasse/libbitcast.git
        GIT_TAG         "main"
        GIT_SHALLOW     1
    )
    FetchContent_MakeAvailable(bitcast_dep)
endif()

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
PRIVATE
    src/read.c

PUBLIC
    FILE_SET myHeaders
    TYPE HEADERS
    BASE_DIRS
        include
    FILES
        include/Reader.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    bitcast
)

# include(FetchContent)
# include(cmake/fetch_gtest.cmake)
# add_subdirectory(test)
# include(CTest)
# enable_testing()
# 
# add_test(NAME bitcastTest
#     COMMAND bitcastTest
# )
