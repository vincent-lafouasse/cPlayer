FetchContent_Declare(
    FileReader
    GIT_REPOSITORY https://github.com/vincent-lafouasse/FileReader.git
    GIT_TAG 7eac11715ca6b897d32e49265d63121edbfb1c40
)
FetchContent_MakeAvailable(FileReader)

FetchContent_Declare(
    bitcast
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libbitcast.git
    GIT_TAG 559fbbedb42bf961eaf8680d2f82dd0a804aa4a5
)
FetchContent_MakeAvailable(bitcast)

add_subdirectory(libstream)
add_subdirectory(libaudiospecs)
