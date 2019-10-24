add_library(recognition-test 
    src/AdaptiveMethod.cpp
    src/FixedLevelMethod.cpp
    src/OutputFileImpl.cpp
    src/OutputFilePathImpl.cpp
    src/RecognitionTestModel_Internal.cpp
    src/RecognitionTestModel.cpp
    src/ResponseEvaluatorImpl.cpp
    src/TrackSettingsInterpreter.cpp
    src/TrackSettingsReader.cpp
)
target_include_directories(recognition-test PUBLIC include)
target_include_directories(recognition-test PRIVATE include/recognition-test)
target_link_libraries(recognition-test av-speech-in-noise GSL)
