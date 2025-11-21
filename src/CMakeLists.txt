add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	main.c
	int24.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ./)
target_link_libraries(${PROJECT_NAME} PRIVATE FileReader)
