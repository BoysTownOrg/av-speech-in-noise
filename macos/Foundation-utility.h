#ifndef AV_SPEECH_IN_NOISE_MACOS_FOUNDATION_UTILITY_H_
#define AV_SPEECH_IN_NOISE_MACOS_FOUNDATION_UTILITY_H_

#include <av-speech-in-noise/Model.hpp>
#import <Foundation/Foundation.h>
#include <string>

namespace av_speech_in_noise {
auto resourceUrl(const std::string &stem, const std::string &extension)
    -> LocalUrl;

auto nsString(const std::string &s) -> NSString *;
}

#endif
