add_library(stimulus-players 
    src/AudioReaderImpl.cpp
    src/MaskerPlayerImpl.cpp
    src/TargetPlayerImpl.cpp
)
target_include_directories(stimulus-players PUBLIC include)
target_include_directories(stimulus-players PRIVATE include/stimulus-players)
target_compile_features(stimulus-players PRIVATE cxx_std_14)
target_link_libraries(stimulus-players recognition-test)
