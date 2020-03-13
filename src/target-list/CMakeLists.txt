add_library(target-list 
    src/FileFilterDecorator.cpp
    src/RandomizedTargetList.cpp
    src/SubdirectoryTargetListReader.cpp
)
target_include_directories(target-list PUBLIC include)
target_include_directories(target-list PRIVATE include/target-list)
target_link_libraries(target-list recognition-test GSL)
