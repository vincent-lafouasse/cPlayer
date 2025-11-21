FetchContent_Declare(
    portaudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

# Optional: disable building tests & tools
set(PA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(portaudio)
