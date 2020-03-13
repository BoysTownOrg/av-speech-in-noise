add_library(presentation
    src/Presenter.cpp
    src/TestSettingsInterpreter.cpp)
target_include_directories(presentation PUBLIC include)
target_include_directories(presentation PRIVATE include/presentation)
target_link_libraries(presentation av-speech-in-noise GSL)
target_compile_options(presentation PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
