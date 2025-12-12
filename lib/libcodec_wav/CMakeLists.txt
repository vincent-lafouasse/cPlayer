cmake_minimum_required(VERSION 3.16)

project(libcodec_wav VERSION 0.1.0)

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
    libaudiospecs
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libaudiospecs.git
    GIT_TAG e2bffa47bcbdee16174bf5f6c9ec435e08f91af3
  )
  FetchContent_MakeAvailable(libaudiospecs)
else()
  message(STATUS "---- Found libaudiospecs in parent project: no fetching")
endif()

if(NOT TARGET libstream)
  message(
    STATUS "---- No libstream here or in parent project: fetching libstream")

  include(FetchContent)
  FetchContent_Declare(
    libstream
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libstream.git
    GIT_TAG d4bc4bbe3918645caffcb36154e0ea7509bbb581
  )
  FetchContent_MakeAvailable(libstream)
else()
  message(STATUS "---- Found libstream in parent project: no fetching")
endif()

if(NOT TARGET bitcast)
  message(
    STATUS "---- No libbitcast here or in parent project: fetching libbitcast")

  include(FetchContent)
  FetchContent_MakeAvailable(bitcast)
  FetchContent_Declare(
    bitcast
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libbitcast.git
    GIT_TAG 559fbbedb42bf961eaf8680d2f82dd0a804aa4a5
  )
else()
  message(STATUS "---- Found libbitcast in parent project: no fetching")
endif()

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
          src/dump.c
          src/formatInfo.c
          src/navigateRiff.c
          src/parseFormatChunk.c
          src/readData.c
          src/readFormatChunk.c
          src/readSample.c
          src/sampleFormat.c
          src/validateFormatChunk.c
          src/wav.c
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE 
    src
  PUBLIC
    include
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic
                                               -fdiagnostics-color=always)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE bitcast
  PUBLIC libstream libaudiospecs)

if(LIBCODEC_WAV_BUILD_TESTS)
  include(FetchContent)
  include(cmake/fetch_gtest.cmake)

  include(CTest)
  enable_testing()

  add_subdirectory(test)
endif()
