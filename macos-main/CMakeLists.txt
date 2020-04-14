find_library(AV_FOUNDATION AVFoundation)
if (NOT AV_FOUNDATION)
    message(FATAL_ERROR "AVFoundation not found")
endif()
find_library(CORE_AUDIO CoreAudio)
if (NOT CORE_AUDIO)
    message(FATAL_ERROR "CoreAudio not found")
endif()
find_library(COCOA Cocoa)
if (NOT COCOA)
    message(FATAL_ERROR "Cocoa not found")
endif()
find_library(CORE_MEDIA CoreMedia)
if (NOT CORE_MEDIA)
    message(FATAL_ERROR "CoreMedia not found")
endif()
find_library(MEDIA_TOOLBOX MediaToolbox)
if (NOT MEDIA_TOOLBOX)
    message(FATAL_ERROR "MediaToolbox not found")
endif()
find_library(AUDIO_TOOLBOX AudioToolbox)
if (NOT AUDIO_TOOLBOX)
    message(FATAL_ERROR "AudioToolbox not found")
endif()
add_executable(AvSpeechInNoise MACOSX_BUNDLE 
    TobiiEyeTracker.cpp
    AvFoundationPlayers.mm
    CocoaView.mm
    main.mm
)
set_target_properties(AvSpeechInNoise PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION} CXX_EXTENSIONS OFF)
target_compile_features(AvSpeechInNoise PRIVATE cxx_std_17)
target_link_libraries(AvSpeechInNoise 
    recognition-test 
    stimulus-players 
    presentation 
    target-list 
    adaptive-track 
    ${AV_FOUNDATION} 
    ${CORE_AUDIO} 
    ${COCOA} 
    ${CORE_MEDIA} 
    ${MEDIA_TOOLBOX}
    ${AUDIO_TOOLBOX}
    GSL
)
target_include_directories(AvSpeechInNoise PRIVATE "/usr/local/include/tobii_research")