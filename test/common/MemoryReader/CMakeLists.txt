add_library(MemoryReader)
target_sources(MemoryReader
PRIVATE
	./MemoryReader.cpp

PUBLIC
	FILE_SET myHeaders
	TYPE HEADERS
	BASE_DIRS
		.
		${CMAKE_SOURCE_DIR}/src/Reader/
		${CMAKE_SOURCE_DIR}/src/common/
	FILES
		./MemoryReader.hpp
)
