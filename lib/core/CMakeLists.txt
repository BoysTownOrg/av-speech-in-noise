add_library(
  av-speech-in-noise-core-lib
  src/AdaptiveMethod.cpp
  src/FixedLevelMethod.cpp
  src/OutputFile.cpp
  src/OutputFilePath.cpp
  src/Model.cpp
  src/RecognitionTestModel.cpp
  src/ResponseEvaluator.cpp
  src/EyeTrackerCalibration.cpp
  src/AdaptiveTrack.cpp
  src/SubmittingFreeResponse.cpp
  src/SubmittingPassFail.cpp)
target_include_directories(
  av-speech-in-noise-core-lib
  PUBLIC include
  PRIVATE include/av-speech-in-noise/core)
target_compile_options(av-speech-in-noise-core-lib
                       PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_compile_features(av-speech-in-noise-core-lib PUBLIC cxx_std_17)
set_target_properties(av-speech-in-noise-core-lib PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(av-speech-in-noise-core-lib
                      av-speech-in-noise-domain-interface GSL)
