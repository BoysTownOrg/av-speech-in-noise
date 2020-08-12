add_executable(
  tests
  AdaptiveMethod.cpp
  AdaptiveTrack.cpp
  AudioReader.cpp
  FixedLevelMethod.cpp
  MaskerPlayer.cpp
  OutputFilePath.cpp
  OutputFile.cpp
  Presenter.cpp
  RecognitionTestModel.cpp
  Model.cpp
  ResponseEvaluator.cpp
  SubdirectoryTargetListReader.cpp
  RandomizedTargetPlaylists.cpp
  TargetPlayer.cpp
  TestSettingsInterpreter.cpp)
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
