function(add_macos_bundle target identifier output_name)
  target_compile_features(${target} PRIVATE cxx_std_17)
  target_compile_options(${target} PRIVATE "-fobjc-arc"
                                           ${AV_SPEECH_IN_NOISE_WARNINGS})
  target_link_libraries(
    ${target}
    recognition-test
    stimulus-players
    presentation
    target-playlists
    adaptive-track
    GSL
    "-framework AppKit -framework AVFoundation -framework CoreMedia -framework MediaToolbox -framework AudioToolbox -framework CoreAudio"
  )
  if(${AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING})
    include(FindPkgConfig)
    pkg_check_modules(TOBIIRESEARCH IMPORTED_TARGET tobii_research) 
    target_link_libraries(${target} PkgConfig::TOBIIRESEARCH)
  endif()
  set_target_properties(
    ${target}
    PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${AV_SPEECH_IN_NOISE_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${AV_SPEECH_IN_NOISE_VERSION}
               MACOSX_BUNDLE_GUI_IDENTIFIER ${identifier}
               RESOURCE "${AV_SPEECH_IN_NOISE_MACOS_RESOURCES}"
               CXX_EXTENSIONS OFF
               OUTPUT_NAME ${output_name}
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
               "${AV_SPEECH_IN_NOISE_MACOS_CODE_SIGN_IDENTITY}"
               XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS
               "${AV_SPEECH_IN_NOISE_MACOS_OTHER_CODE_SIGN_FLAGS}"
               XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS NO)
endfunction()

option(AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING "Enables tobii eye tracking"
       OFF)
if(${AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING})
  set(MAIN_IMPL_FILE main/with-tobii.mm)
else()
  set(MAIN_IMPL_FILE main/no-eyetracking.mm)
endif()

set(AV_SPEECH_IN_NOISE_MACOS_RESOURCES
    ""
    CACHE STRING "Resources for AV Speech in Noise")
set(AV_SPEECH_IN_NOISE_MACOS_CODE_SIGN_IDENTITY
    ""
    CACHE STRING "Xcode Code Signing Identity")
set(AV_SPEECH_IN_NOISE_MACOS_OTHER_CODE_SIGN_FLAGS
    ""
    CACHE STRING "Xcode Other Code Signing Flags")

add_executable(
  av-speech-in-noise-bundle MACOSX_BUNDLE
  AvFoundationPlayers.mm
  AppKitView.mm
  AppKit-utility.mm
  Foundation-utility.mm
  run.mm
  ${MAIN_IMPL_FILE}
  ${AV_SPEECH_IN_NOISE_MACOS_RESOURCES})
add_macos_bundle(av-speech-in-noise-bundle org.boystown.av-speech-in-noise
                 "AV Speech in Noise")
add_executable(
  av-speech-in-noise-facemask-study MACOSX_BUNDLE
  AvFoundationPlayers.mm
  AppKitView.mm
  AppKit-utility.mm
  Foundation-utility.mm
  FacemaskStudySetupView.mm
  run.mm
  main/facemask-study.mm
  ${AV_SPEECH_IN_NOISE_MACOS_RESOURCES})
add_macos_bundle(
  av-speech-in-noise-facemask-study
  org.boystown.av-speech-in-noise-facemask-study "Facemask Study")
add_library(boystown-av-speech-in-noise-swift hi.swift)
set_target_properties(boystown-av-speech-in-noise-swift PROPERTIES Swift_MODULE_NAME BoystownAvSpeechInNoiseSwift)
