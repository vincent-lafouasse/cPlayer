set(libName DeserializeInts)
add_library(${libName})

target_sources(${libName}
PRIVATE
	src/DeserializeInts.c

PUBLIC
	FILE_SET myHeaders
	TYPE HEADERS
	BASE_DIRS
		include
	FILES
		include/DeserializeInts.h
)

