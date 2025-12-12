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

FetchContent_Declare(
    libaudiospecs
    GIT_REPOSITORY https://github.com/vincent-lafouasse/libaudiospecs.git
    GIT_TAG e2bffa47bcbdee16174bf5f6c9ec435e08f91af3
)
FetchContent_MakeAvailable(libaudiospecs)

add_subdirectory(libstream)
