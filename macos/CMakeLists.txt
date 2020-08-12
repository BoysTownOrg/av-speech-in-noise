find_library(AV_FOUNDATION AVFoundation)
if(NOT AV_FOUNDATION)
  message(FATAL_ERROR "AVFoundation not found")
endif()
find_library(CORE_AUDIO CoreAudio)
if(NOT CORE_AUDIO)
  message(FATAL_ERROR "CoreAudio not found")
endif()
find_library(COCOA Cocoa)
if(NOT COCOA)
  message(FATAL_ERROR "Cocoa not found")
endif()
find_library(CORE_MEDIA CoreMedia)
if(NOT CORE_MEDIA)
  message(FATAL_ERROR "CoreMedia not found")
endif()
find_library(MEDIA_TOOLBOX MediaToolbox)
if(NOT MEDIA_TOOLBOX)
  message(FATAL_ERROR "MediaToolbox not found")
endif()
find_library(AUDIO_TOOLBOX AudioToolbox)
if(NOT AUDIO_TOOLBOX)
  message(FATAL_ERROR "AudioToolbox not found")
endif()

option(AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING "Enables tobii eye tracking"
       OFF)
if(${AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING})
  set(MAIN_IMPL_FILE main-tobii.mm)
else()
  set(MAIN_IMPL_FILE main-no-eyetracking.mm)
endif()

set(AV_SPEECH_IN_NOISE_MACOS_RESOURCES
    ""
    CACHE STRING "Resources for AV Speech in Noise")
add_executable(
  AvSpeechInNoise MACOSX_BUNDLE
  AvFoundationPlayers.mm CocoaView.mm ${MAIN_IMPL_FILE} main.mm
  ${AV_SPEECH_IN_NOISE_MACOS_RESOURCES})
target_compile_features(AvSpeechInNoise PRIVATE cxx_std_17)
target_compile_options(AvSpeechInNoise PRIVATE ${AV_SPEECH_IN_NOISE_WARNINGS})
target_link_libraries(
  AvSpeechInNoise
  recognition-test
  stimulus-players
  presentation
  target-playlists
  adaptive-track
  ${AV_FOUNDATION}
  ${CORE_AUDIO}
  ${COCOA}
  ${CORE_MEDIA}
  ${MEDIA_TOOLBOX}
  ${AUDIO_TOOLBOX}
  GSL)
target_include_directories(AvSpeechInNoise
                           PRIVATE "/usr/local/include/tobii_research")
set(AV_SPEECH_IN_NOISE_MACOS_CODE_SIGN_IDENTITY
    ""
    CACHE STRING "Xcode Code Signing Identity")
set(AV_SPEECH_IN_NOISE_MACOS_OTHER_CODE_SIGN_FLAGS
    ""
    CACHE STRING "Xcode Other Code Signing Flags")
set_target_properties(
  AvSpeechInNoise
  PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
             MACOSX_BUNDLE_GUI_IDENTIFIER org.boystown.av-speech-in-noise
             RESOURCE "${AV_SPEECH_IN_NOISE_MACOS_RESOURCES}"
             CXX_EXTENSIONS OFF
             OUTPUT_NAME "AV Speech in Noise"
             XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
             "${AV_SPEECH_IN_NOISE_MACOS_CODE_SIGN_IDENTITY}"
             XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS
             "${AV_SPEECH_IN_NOISE_MACOS_OTHER_CODE_SIGN_FLAGS}"
             XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS NO)
