set(
    SOURCES
    "src/OutputFileImpl.cpp"
    "src/OutputFilePathImpl.cpp"
    "src/RecognitionTestModel.cpp"
    "src/ResponseEvaluatorImpl.cpp"
)

add_library(recognition-test ${SOURCES})
target_include_directories(recognition-test PUBLIC include)
target_include_directories(recognition-test PRIVATE include/recognition-test)
target_compile_options(recognition-test PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(recognition-test av-speech-in-noise GSL)

