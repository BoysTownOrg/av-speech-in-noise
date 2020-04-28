add_library(
  target-playlists src/FileFilterDecorator.cpp src/RandomizedTargetPlaylists.cpp
              src/SubdirectoryTargetPlaylistReader.cpp)
target_include_directories(
  target-playlists
  PUBLIC include
  PRIVATE include/target-playlists)
target_compile_features(target-playlists PUBLIC cxx_std_11)
target_compile_options(target-playlists PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
set_target_properties(target-playlists PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(target-playlists recognition-test GSL)
