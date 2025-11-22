set(libName bitcast)
add_library(${libName})

target_sources(${libName}
PRIVATE
	src/bitcast.c

PUBLIC
	FILE_SET myHeaders
	TYPE HEADERS
	BASE_DIRS
		include
	FILES
		include/bitcast.h
)

