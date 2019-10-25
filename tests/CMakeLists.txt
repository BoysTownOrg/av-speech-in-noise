add_executable(
    tests AdaptiveMethodTests.cpp AdaptiveTrackTests.cpp AudioReaderTests
        .cpp FixedLevelMethodTests.cpp MaskerPlayerTests.cpp OutputFilePathTests
        .cpp OutputFileTests.cpp PresenterTests
        .cpp RecognitionTestModel_InternalTests.cpp RecognitionTestModelTests
        .cpp ResponseEvaluatorTests.cpp SubdirectoryTargetListReaderTests
        .cpp TargetListTests.cpp TargetPlayerTests
        .cpp TrackSettingsInterpreterTests.cpp TrackSettingsReaderTests.cpp)
    target_compile_features(tests PRIVATE cxx_std_17)
        target_link_libraries(tests recognition - test adaptive -
            track presentation stimulus - players target - list gtest_main)
