function(add_macos_bundle target identifier output_name)
  add_executable(
    ${target} MACOSX_BUNDLE
    AvFoundationPlayers.mm
    AppKitView.mm
    AppKit-utility.mm
    Foundation-utility.mm
    run.mm
    utility.mm
    objective-c-adapters.mm
    SwiftUI.swift
    ${ARGN}
    ${AV_SPEECH_IN_NOISE_MACOS_RESOURCES})
  target_compile_features(${target} PRIVATE cxx_std_17)
  target_link_libraries(
    ${target}
    av-speech-in-noise-core-lib
    av-speech-in-noise-player-lib
    av-speech-in-noise-ui-lib
    av-speech-in-noise-playlist-lib
    GSL
    "-framework AppKit -framework AVFoundation -framework CoreMedia -framework MediaToolbox -framework AudioToolbox -framework CoreAudio"
  )
  set_target_properties(
    ${target}
    PROPERTIES MACOSX_BUNDLE_INFO_PLIST
               ${CMAKE_SOURCE_DIR}/MacOSXBundleInfo.plist.in
               MACOSX_BUNDLE_BUNDLE_VERSION "${AV_SPEECH_IN_NOISE_VERSION}"
               MACOSX_BUNDLE_SHORT_VERSION_STRING
               "${AV_SPEECH_IN_NOISE_VERSION}"
               MACOSX_BUNDLE_GUI_IDENTIFIER ${identifier}
               XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${identifier}
               RESOURCE "${AV_SPEECH_IN_NOISE_MACOS_RESOURCES}"
               CXX_EXTENSIONS OFF
               OUTPUT_NAME ${output_name}
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
               "${AV_SPEECH_IN_NOISE_MACOS_CODE_SIGN_IDENTITY}"
               XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS
               "${AV_SPEECH_IN_NOISE_MACOS_OTHER_CODE_SIGN_FLAGS}"
               XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS NO
               XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER
               ${CMAKE_CURRENT_SOURCE_DIR}/objective-c-bridge.h)
endfunction()

option(AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING "Enables tobii eye tracking"
       OFF)
if(${AV_SPEECH_IN_NOISE_ENABLE_TOBII_EYETRACKING})
  include(FindPkgConfig)
  pkg_check_modules(TOBIIRESEARCH IMPORTED_TARGET tobii_research)
  add_macos_bundle(
    av-speech-in-noise-with-tobii-macos-bundle
    org.boystown.av-speech-in-noise-with-tobii "AV Speech in Noise"
    main/with-tobii.mm main/with-tobii.swift TobiiProEyeTracker.cpp)
  target_link_libraries(av-speech-in-noise-with-tobii-macos-bundle
                        PkgConfig::TOBIIRESEARCH)
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
add_macos_bundle(
  av-speech-in-noise-macos-bundle org.boystown.av-speech-in-noise
  "AV Speech in Noise" main/default.mm main/default.swift)
add_macos_bundle(
  av-speech-in-noise-facemask-study-macos-bundle
  org.boystown.av-speech-in-noise-facemask-study "Facemask Study"
  main/facemask-study.swift main/facemask-study.mm)
set_source_files_properties(
  AvFoundationPlayers.mm
  AppKitView.mm
  AppKit-utility.mm
  Foundation-utility.mm
  run.mm
  utility.mm
  objective-c-adapters.mm
  main/default.mm
  main/facemask-study.mm
  main/with-tobii.mm
  PROPERTIES COMPILE_OPTIONS "-fobjc-arc;${AV_SPEECH_IN_NOISE_WARNINGS}")
set_source_files_properties(
  TobiiProEyeTracker.cpp PROPERTIES COMPILE_OPTIONS
                                    "${AV_SPEECH_IN_NOISE_WARNINGS}")
