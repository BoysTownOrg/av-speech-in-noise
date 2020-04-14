add_library(stimulus-players 
    src/AudioReaderImpl.cpp
    src/MaskerPlayerImpl.cpp
    src/TargetPlayerImpl.cpp
)
target_include_directories(stimulus-players PUBLIC include PRIVATE include/stimulus-players)
target_compile_features(stimulus-players PUBLIC cxx_std_11)
set_target_properties(stimulus-players PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(stimulus-players recognition-test GSL)
