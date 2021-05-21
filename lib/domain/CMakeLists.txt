add_library(av-speech-in-noise-domain-interface INTERFACE)
target_include_directories(av-speech-in-noise-domain-interface
                           INTERFACE include)
target_compile_features(av-speech-in-noise-domain-interface
                        INTERFACE cxx_std_17)
set_target_properties(av-speech-in-noise-domain-interface
                      PROPERTIES CXX_EXTENSIONS OFF)
