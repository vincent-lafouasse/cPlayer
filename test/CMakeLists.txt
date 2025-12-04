set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

add_subdirectory(common/MemoryReader)

add_subdirectory(i24)
add_subdirectory(Options)
add_subdirectory(Reader)
