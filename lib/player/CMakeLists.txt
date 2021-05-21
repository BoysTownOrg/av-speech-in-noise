add_library(
  av-speech-in-noise-player-lib
  src/AudioReaderSimplified.cpp src/MaskerPlayerImpl.cpp
  src/TargetPlayerImpl.cpp)
target_include_directories(
  av-speech-in-noise-player-lib
  PUBLIC include
  PRIVATE include/av-speech-in-noise/player)
target_compile_features(av-speech-in-noise-player-lib PUBLIC cxx_std_11)
target_compile_options(av-speech-in-noise-player-lib
                       PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
set_target_properties(av-speech-in-noise-player-lib PROPERTIES CXX_EXTENSIONS
                                                               OFF)
target_link_libraries(av-speech-in-noise-player-lib av-speech-in-noise-core-lib
                      GSL)
