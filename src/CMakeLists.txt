add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c

	Reader/fromFileReader.c
	Reader/fromFixedBufferReader.c

	codec/decode.c
	codec/wav/wav.c
	codec/wav/read.c
	codec/wav/header.c
	codec/wav/dump.c
	codec/wav/sampleFormat.c
	codec/codec_internals.c

	play/callback.c

	options/Options.c
	options/Flag.c
	options/printHelp.c

	common/int24.c
	common/Error.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
	./
	./common/
	${portaudio_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
	FileReader
	bitcast
	portaudio
)
