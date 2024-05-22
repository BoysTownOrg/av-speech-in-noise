add_library(
  av-speech-in-noise-ui-lib
  src/SessionControllerImpl.cpp
  src/TestSettingsInterpreter.cpp
  src/Consonant.cpp
  src/KeyPress.cpp
  src/FreeResponse.cpp
  src/CorrectKeywords.cpp
  src/CoordinateResponseMeasure.cpp
  src/PassFail.cpp
  src/TestImpl.cpp
  src/TestSetupImpl.cpp
  src/ChooseKeywords.cpp
  src/Syllables.cpp
  src/EyeTrackerCalibration.cpp
  src/RevealImage.cpp)
target_include_directories(
  av-speech-in-noise-ui-lib
  PUBLIC include
  PRIVATE include/av-speech-in-noise/ui)
target_link_libraries(
  av-speech-in-noise-ui-lib av-speech-in-noise-domain-interface
  av-speech-in-noise-core-lib GSL)
target_compile_options(av-speech-in-noise-ui-lib
                       PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_compile_features(av-speech-in-noise-ui-lib PUBLIC cxx_std_17)
set_target_properties(av-speech-in-noise-ui-lib PROPERTIES CXX_EXTENSIONS OFF)
