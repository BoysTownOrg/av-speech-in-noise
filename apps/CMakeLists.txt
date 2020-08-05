if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_library(AV_FOUNDATION AVFoundation)
  if(NOT AV_FOUNDATION)
    message(FATAL_ERROR "AVFoundation not found")
  endif()
  find_library(CORE_AUDIO CoreAudio)
  if(NOT CORE_AUDIO)
    message(CORE_AUDIO "CoreAudio not found")
  endif()
  find_library(COCOA Cocoa)
  if(NOT COCOA)
    message(COCOA "Cocoa not found")
  endif()
  find_library(CORE_MEDIA CoreMedia)
  if(NOT CORE_MEDIA)
    message(CORE_MEDIA "CoreMedia not found")
  endif()
  find_library(MEDIA_TOOLBOX MediaToolbox)
  if(NOT MEDIA_TOOLBOX)
    message(MEDIA_TOOLBOX "MediaToolbox not found")
  endif()
  find_library(AUDIO_TOOLBOX AudioToolbox)
  if(NOT AUDIO_TOOLBOX)
    message(AUDIO_TOOLBOX "AudioToolbox not found")
  endif()
  add_executable(AvSpeechInNoise MACOSX_BUNDLE AvFoundationPlayers.mm
                                               CocoaView.mm main.mm)
  target_compile_features(AvSpeechInNoise PRIVATE cxx_std_17)
  target_link_libraries(
    AvSpeechInNoise
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
    ${AUDIO_TOOLBOX})
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
               CXX_EXTENSIONS OFF
               OUTPUT_NAME "AV Speech in Noise"
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
               "${AV_SPEECH_IN_NOISE_MACOS_CODE_SIGN_IDENTITY}"
               XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS
               "${AV_SPEECH_IN_NOISE_MACOS_OTHER_CODE_SIGN_FLAGS}"
               XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS NO)
endif()
