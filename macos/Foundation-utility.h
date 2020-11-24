#ifndef AV_SPEECH_IN_NOISE_MACOS_FOUNDATION_UTILITY_H_
#define AV_SPEECH_IN_NOISE_MACOS_FOUNDATION_UTILITY_H_

#import <Foundation/Foundation.h>
#include <string>

auto resourcePath(const std::string &stem, const std::string &extension)
    -> std::string;

#endif
