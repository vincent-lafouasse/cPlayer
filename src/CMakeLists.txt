add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c

	wav/wav.c
	wav/read.c
	wav/header.c
	wav/dump.c
	wav/sampleFormat.c

	common/int24.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
	./
	./common/
	${portaudio_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
	FileReader
	DeserializeInts
	portaudio
)
