set(
    SOURCES
    "src/Presenter.cpp"
)

add_library(presentation ${SOURCES})
target_include_directories(presentation PUBLIC include)
target_include_directories(presentation PRIVATE include/presentation)
target_compile_options(presentation PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(presentation av-speech-in-noise)
