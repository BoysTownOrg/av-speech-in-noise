cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(AvSpeechInNoise VERSION 2.0.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(GSL)
add_subdirectory(av-speech-in-noise)
add_subdirectory(apps)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)
add_subdirectory(tests)
