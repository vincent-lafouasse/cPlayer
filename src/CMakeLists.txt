add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c

	codec/decode.c
	codec/wav/wav.c
	codec/wav/read.c
	codec/wav/header.c
	codec/wav/dump.c
	codec/wav/sampleFormat.c

	play/callback.c

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
