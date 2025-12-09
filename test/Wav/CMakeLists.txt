set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME WavTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(
  ${TEST_RUNNER_NAME}
  PRIVATE ./main.cpp
          ./readFormat.cpp
          ./validateFormat.cpp
          ${testedSourceDir}/codec/wav/navigateRiff.c
          ${testedSourceDir}/codec/wav/readFormatChunk.c
          ${testedSourceDir}/codec/wav/validateFormatChunk.c
          ${testedSourceDir}/codec/wav/parseFormatChunk.c
          ${testedSourceDir}/codec/wav/formatInfo.c
          ${testedSourceDir}/codec/wav/sampleFormat.c
          ${testedSourceDir}/Reader/read.c
          ${testedSourceDir}/common/Error.c)

target_include_directories(
  ${TEST_RUNNER_NAME} PUBLIC ${testedSourceDir}/ ${testedSourceDir}/codec/wav/
                             ${testedSourceDir}/common/)

target_link_libraries(${TEST_RUNNER_NAME} PRIVATE MemoryReader bitcast gtest)

add_test(NAME ${TEST_RUNNER_NAME} COMMAND ${TEST_RUNNER_NAME})

set_tests_properties(${TEST_RUNNER_NAME} PROPERTIES DEPENDS bitcastTest)
