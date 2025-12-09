add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE main.c
          codec/decode.c
          codec/codec_internals.c
          codec/wav/wav.c
          codec/wav/navigateRiff.c
          codec/wav/readFormatChunk.c
          codec/wav/validateFormatChunk.c
          codec/wav/parseFormatChunk.c
          codec/wav/formatInfo.c
          codec/wav/sampleFormat.c
          codec/wav/readSample.c
          codec/wav/readData.c
          codec/wav/dump.c
          play/callback.c
          Reader/fileReaderAdapter.c
          options/Options.c
          options/Flag.c
          options/printHelp.c
          common/AudioData.c
          common/Error.c
          common/logError.c)

target_include_directories(${PROJECT_NAME} PUBLIC ./ ./common/)
target_link_libraries(${PROJECT_NAME} PRIVATE FileReader bitcast libstream
                                              portaudio)
