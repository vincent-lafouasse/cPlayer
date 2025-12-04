set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME WavTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(${TEST_RUNNER_NAME}
PRIVATE
	./main.cpp
	./WavHeaderTest.cpp

	${testedSourceDir}/codec/wav/header.c
	${testedSourceDir}/codec/wav/sampleFormat.c

	${testedSourceDir}/Reader/read.c

	${testedSourceDir}/common/int24.c
)

target_include_directories(${TEST_RUNNER_NAME} PUBLIC
	${testedSourceDir}/
	${testedSourceDir}/codec/wav/
	${testedSourceDir}/common/
)

target_link_libraries(${TEST_RUNNER_NAME}
PRIVATE
	MemoryReader
	bitcast
	gtest
)
