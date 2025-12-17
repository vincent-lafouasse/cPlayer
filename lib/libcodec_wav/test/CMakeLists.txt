set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

set(testedSourceDir ../src)
set(testedIncludeDir ../include)

add_subdirectory(MemoryReader)

set(TEST_RUNNER_NAME WavTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(
  ${TEST_RUNNER_NAME}
  PRIVATE ./main.cpp
          ./readFormat.cpp
          ./validateFormat.cpp
)

target_include_directories(${TEST_RUNNER_NAME} PUBLIC ${testedSourceDir}/)

target_link_libraries(${TEST_RUNNER_NAME} PRIVATE MemoryReader libcodec_wav gtest)

add_test(NAME ${TEST_RUNNER_NAME} COMMAND ${TEST_RUNNER_NAME})

set_tests_properties(${TEST_RUNNER_NAME} PROPERTIES DEPENDS bitcastTest)
set_tests_properties(${TEST_RUNNER_NAME} PROPERTIES DEPENDS ReaderTest)
