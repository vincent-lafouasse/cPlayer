add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE main.c
          codec/decode.c
          play/callback.c
          Reader/fileReaderAdapter.c
          options/Options.c
          options/Flag.c
          options/printHelp.c
          common/Error.c
          common/logError.c)

target_include_directories(${PROJECT_NAME} PUBLIC ./ ./common/)
target_link_libraries(${PROJECT_NAME} PRIVATE FileReader libcodec_wav portaudio)
