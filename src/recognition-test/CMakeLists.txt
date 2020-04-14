add_library(recognition-test 
    src/AdaptiveMethod.cpp
    src/FixedLevelMethod.cpp
    src/OutputFile.cpp
    src/OutputFilePath.cpp
    src/Model.cpp
    src/RecognitionTestModel.cpp
    src/ResponseEvaluator.cpp
)
target_include_directories(recognition-test PUBLIC include PRIVATE include/recognition-test)
target_compile_options(recognition-test PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_compile_features(recognition-test PUBLIC cxx_std_11)
set_target_properties(recognition-test PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(recognition-test av-speech-in-noise GSL)
