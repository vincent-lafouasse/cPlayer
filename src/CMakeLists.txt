add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c
	wav.c
	int24.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
	./
	${portaudio_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
	FileReader
	portaudio
)
