add_library(
  av-speech-in-noise-playlist-lib
  src/FileFilterDecorator.cpp src/RandomizedTargetPlaylists.cpp
  src/SubdirectoryTargetPlaylistReader.cpp src/PredeterminedTargetPlaylist.cpp)
target_include_directories(
  av-speech-in-noise-playlist-lib
  PUBLIC include
  PRIVATE include/av-speech-in-noise/playlist)
target_compile_features(av-speech-in-noise-playlist-lib PUBLIC cxx_std_11)
target_compile_options(av-speech-in-noise-playlist-lib
                       PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
set_target_properties(av-speech-in-noise-playlist-lib PROPERTIES CXX_EXTENSIONS
                                                                 OFF)
target_link_libraries(av-speech-in-noise-playlist-lib
                      av-speech-in-noise-core-lib GSL)
