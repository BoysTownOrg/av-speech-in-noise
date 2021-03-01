cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(
  AvSpeechInNoise
  VERSION 4.1.0
  LANGUAGES CXX OBJCXX)

include(FetchContent)

FetchContent_Declare(
  GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL
  GIT_TAG v3.1.0)
FetchContent_MakeAvailable(GSL)

add_subdirectory(lib)
add_subdirectory(macos)

option(AV_SPEECH_IN_NOISE_ENABLE_TESTS
       "Enables tests for av-speech-in-noise target" OFF)
if(${AV_SPEECH_IN_NOISE_ENABLE_TESTS})
  enable_testing()

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.10.0)
  FetchContent_MakeAvailable(googletest)

  add_subdirectory(test)
endif()
