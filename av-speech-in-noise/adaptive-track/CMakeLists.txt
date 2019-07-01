set(
    SOURCES
    "src/AdaptiveTrack.cpp"
)

add_library(adaptive-track ${SOURCES})
target_include_directories(adaptive-track PUBLIC include)
target_include_directories(adaptive-track PRIVATE include/adaptive-track)
target_link_libraries(adaptive-track recognition-test)

