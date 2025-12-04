set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME ReaderTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(${TEST_RUNNER_NAME}
PRIVATE
	./main.cpp
	./MemoryReaderReaderTest.cpp
	./FileReaderReaderTest.cpp

	${testedSourceDir}/Reader/fileReaderAdapter.c
)

target_include_directories(${TEST_RUNNER_NAME} PUBLIC
	${testedSourceDir}/Reader/
)

target_link_libraries(${TEST_RUNNER_NAME}
PRIVATE
	MemoryReader
	FileReader
	gtest
)
