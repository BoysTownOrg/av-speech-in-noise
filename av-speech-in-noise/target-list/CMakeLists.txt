set(
    SOURCES
    "src/FileExtensionFilterDecorator.cpp"
    "src/RandomizedTargetList.cpp"
    "src/SubdirectoryTargetListReader.cpp"
)

add_library(target-list ${SOURCES})
target_include_directories(target-list PUBLIC include)
target_include_directories(target-list PRIVATE include/target-list)
target_link_libraries(target-list recognition-test)

