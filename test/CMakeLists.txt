add_executable(
  av-speech-in-noise-tests
  AdaptiveMethod.cpp
  AdaptiveTrack.cpp
  AudioReaderSimplified.cpp
  FixedLevelMethod.cpp
  MaskerPlayer.cpp
  OutputFilePath.cpp
  OutputFile.cpp
  RecognitionTestModel.cpp
  Model.cpp
  ResponseEvaluator.cpp
  SubdirectoryTargetListReader.cpp
  RandomizedTargetPlaylists.cpp
  TargetPlayer.cpp
  TestSettingsInterpreter.cpp
  Consonant.cpp
  CoordinateResponseMeasure.cpp
  CorrectKeywords.cpp
  FreeResponse.cpp
  PassFail.cpp
  Test.cpp
  TestSetup.cpp
  SessionController.cpp
  ChooseKeywords.cpp)
target_compile_features(av-speech-in-noise-tests PRIVATE cxx_std_17)
set_target_properties(av-speech-in-noise-tests PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(av-speech-in-noise-tests
                       PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_link_libraries(
  av-speech-in-noise-tests
  recognition-test
  adaptive-track
  presentation
  stimulus-players
  target-playlists
  gtest_main
  GSL)
add_test(NAME av-speech-in-noise-tests COMMAND av-speech-in-noise-tests)
