add_library(
  presentation
  src/SessionControllerImpl.cpp
  src/TestSettingsInterpreter.cpp
  src/Consonant.cpp
  src/FreeResponse.cpp
  src/CorrectKeywords.cpp
  src/CoordinateResponseMeasure.cpp
  src/PassFail.cpp
  src/TestImpl.cpp
  src/TestSetupImpl.cpp)
target_include_directories(
  presentation
  PUBLIC include
  PRIVATE include/presentation)
target_link_libraries(presentation av-speech-in-noise GSL)
target_compile_options(presentation PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_compile_features(presentation PUBLIC cxx_std_17)
set_target_properties(presentation PROPERTIES CXX_EXTENSIONS OFF)
