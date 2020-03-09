add_library(recognition-test 
    src/AdaptiveMethod.cpp
    src/FixedLevelMethod.cpp
    src/OutputFile.cpp
    src/OutputFilePath.cpp
    src/Model.cpp
    src/RecognitionTestModel.cpp
    src/ResponseEvaluator.cpp
)
target_include_directories(recognition-test PUBLIC include)
target_include_directories(recognition-test PRIVATE include/recognition-test)
target_link_libraries(recognition-test av-speech-in-noise GSL)
