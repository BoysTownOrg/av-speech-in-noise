add_library(
  target-list src/FileFilterDecorator.cpp src/RandomizedTargetList.cpp
              src/SubdirectoryTargetListReader.cpp)
target_include_directories(
  target-list
  PUBLIC include
  PRIVATE include/target-list)
target_compile_features(target-list PUBLIC cxx_std_11)
target_compile_options(target-list PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
set_target_properties(target-list PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(target-list recognition-test GSL)
