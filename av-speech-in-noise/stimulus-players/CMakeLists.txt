set(
    SOURCES
    "src/AudioReaderImpl.cpp"
    "src/MaskerPlayerImpl.cpp"
    "src/TargetPlayerImpl.cpp"
)

add_library(stimulus-players ${SOURCES})
target_include_directories(stimulus-players PUBLIC include)
target_include_directories(stimulus-players PRIVATE include/stimulus-players)
target_compile_features(stimulus-players PRIVATE cxx_std_14)
target_compile_options(stimulus-players PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(stimulus-players recognition-test)
