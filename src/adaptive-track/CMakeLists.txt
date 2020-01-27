add_library(adaptive-track src/AdaptiveTrack.cpp)
target_include_directories(adaptive-track PUBLIC include)
target_include_directories(adaptive-track PRIVATE include/adaptive-track)
target_link_libraries(adaptive-track recognition-test)
