add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c
	FileReader.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ./)
