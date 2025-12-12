FetchContent_Declare(
    FileReader
    GIT_REPOSITORY https://github.com/vincent-lafouasse/FileReader.git
    GIT_TAG 7eac11715ca6b897d32e49265d63121edbfb1c40
)
FetchContent_MakeAvailable(FileReader)

add_subdirectory(libbitcast)
add_subdirectory(libstream)
add_subdirectory(libaudiospecs)
