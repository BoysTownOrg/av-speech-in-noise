find_library(AV_FOUNDATION AVFoundation)
if (NOT AV_FOUNDATION)
    message(FATAL_ERROR "AVFoundation not found")
endif()
find_library(CORE_AUDIO CoreAudio)
if (NOT CORE_AUDIO)
    message(CORE_AUDIO "CoreAudio not found")
endif()
find_library(COCOA Cocoa)
if (NOT COCOA)
    message(COCOA "Cocoa not found")
endif()
find_library(CORE_MEDIA CoreMedia)
if (NOT CORE_MEDIA)
    message(CORE_MEDIA "CoreMedia not found")
endif()
find_library(MEDIA_TOOLBOX MediaToolbox)
if (NOT MEDIA_TOOLBOX)
    message(MEDIA_TOOLBOX "MediaToolbox not found")
endif()
add_executable(AvSpeechInNoise MACOSX_BUNDLE 
    AvFoundationPlayers.mm
    CocoaView.mm
    main.mm
)
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
)
