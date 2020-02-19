cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(AvSpeechInNoise VERSION 2.4.1 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(GSL)
add_subdirectory(src)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_subdirectory(macos-main)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows") 
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)
add_subdirectory(test)
