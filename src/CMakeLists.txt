add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c

	codec/decode.c
	codec/wav/wav.c
	codec/wav/read.c
	codec/wav/formatInfo.c
	codec/wav/readData.c
	codec/wav/dump.c
	codec/wav/sampleFormat.c
	codec/codec_internals.c

	play/callback.c

	Reader/read.c
	Reader/fileReaderAdapter.c

	options/Options.c
	options/Flag.c
	options/printHelp.c

	common/int24.c
	common/logError.c
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
