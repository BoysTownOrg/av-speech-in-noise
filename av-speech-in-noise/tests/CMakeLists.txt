set(
	TESTS
	"AdaptiveTrackTests.cpp"
	"AudioReaderTests.cpp"
	"MaskerPlayerTests.cpp"
	"OutputFilePathTests.cpp"
	"OutputFileTests.cpp"
	"PresenterTests.cpp"
	"RecognitionTestModelTests.cpp"
	"ResponseEvaluatorTests.cpp"
	"SubdirectoryTargetListReaderTests.cpp"
	"TargetListTests.cpp"
	"TargetPlayerTests.cpp"
)
add_executable(tests ${TESTS})
target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(tests recognition-test adaptive-track presentation stimulus-players target-list gtest_main)
