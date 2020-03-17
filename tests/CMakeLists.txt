add_executable(tests 
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
	RandomizedTargetListTests.cpp
	TargetPlayerTests.cpp
	TestSettingsInterpreterTests.cpp
)
target_compile_features(tests PRIVATE cxx_std_17)
target_compile_options(tests PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_link_libraries(tests 
	recognition-test 
	adaptive-track 
	presentation 
	stimulus-players 
	target-list 
	gtest_main
	GSL
)
