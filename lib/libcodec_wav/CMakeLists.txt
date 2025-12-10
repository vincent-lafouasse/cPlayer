cmake_minimum_required(VERSION 3.16)

project(libcoded_wav VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(LIBCODEC_WAV_BUILD_TESTS "Build and run tests using CTest" ON)

if(NOT TARGET libaudiospecs)
  message(
    STATUS
      "---- No libaudiospecs here or in parent project: fetching libaudiospecs")

  include(FetchContent)
  FetchContent_Declare(
    libaudiospecs_dep
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libaudiospecs.git
    GIT_TAG "main"
    GIT_SHALLOW 1)
  FetchContent_MakeAvailable(libaudiospecs_dep)
else()
  message(STATUS "---- Found libaudiospecs in parent project: no fetching")
endif()

if(NOT TARGET libstream)
  message(
    STATUS "---- No libstream here or in parent project: fetching libstream")

  include(FetchContent)
  FetchContent_Declare(
    libstream_dep
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libstream.git
    GIT_TAG "main"
    GIT_SHALLOW 1)
  FetchContent_MakeAvailable(libstream_dep)
else()
  message(STATUS "---- Found libstream in parent project: no fetching")
endif()

if(NOT TARGET bitcast)
  message(
    STATUS "---- No libbitcast here or in parent project: fetching libbitcast")

  include(FetchContent)
  FetchContent_Declare(
    bitcast_dep
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libbitcast.git
    GIT_TAG "main"
    GIT_SHALLOW 1)
  FetchContent_MakeAvailable(bitcast_dep)
else()
  message(STATUS "---- Found libbitcast in parent project: no fetching")
endif()

add_library(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PRIVATE src/dump.c
          src/formatInfo.c
          src/navigateRiff.c
          src/parseFormatChunk.c
          src/readData.c
          src/readFormatChunk.c
          src/readSample.c
          src/sampleFormat.c
          src/validateFormatChunk.c
          src/wav.c
  PUBLIC FILE_SET
         myHeaders
         TYPE
         HEADERS
         BASE_DIRS
         include
         FILES
         include/libcodec_wav.h)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic
                                               -fdiagnostics-color=always)

target_link_libraries(${PROJECT_NAME} PRIVATE bitcast libstream libaudiospecs)
