add_library(av-speech-in-noise INTERFACE)
target_include_directories(av-speech-in-noise INTERFACE include)
target_compile_features(av-speech-in-noise INTERFACE cxx_std_17)
set_target_properties(av-speech-in-noise PROPERTIES CXX_EXTENSIONS OFF)
