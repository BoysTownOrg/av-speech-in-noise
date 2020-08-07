add_executable(
  tests
  AdaptiveMethodTests.cpp
  AdaptiveTrackTests.cpp
  AudioReaderTests.cpp
  FixedLevelMethodTests.cpp
  MaskerPlayerTests.cpp
  OutputFilePathTests.cpp
  OutputFileTests.cpp
  PresenterTests.cpp
  RecognitionTestModelTests.cpp
  ModelTests.cpp
  ResponseEvaluatorTests.cpp
  SubdirectoryTargetListReaderTests.cpp
  RandomizedTargetPlaylistsTests.cpp
  TargetPlayerTests.cpp
  TestSettingsInterpreterTests.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(tests PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_link_libraries(
  tests
  recognition-test
  adaptive-track
  presentation
  stimulus-players
  target-playlists
  gtest_main
  GSL)
add_test(NAME tests COMMAND tests)
