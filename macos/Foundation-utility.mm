#include "Foundation-utility.h"

namespace av_speech_in_noise {
auto resourceUrl(const std::string &stem, const std::string &extension)
    -> LocalUrl {
    const auto resourcePath{
        [[NSBundle mainBundle] pathForResource:nsString(stem)
                                        ofType:nsString(extension)]};
    return LocalUrl{resourcePath != nil ? resourcePath.UTF8String : ""};
}

auto nsString(const std::string &s) -> NSString * {
    // https://developer.apple.com/documentation/foundation/nsstring/1497379-stringwithutf8string?language=objc
    // "Raises an exception if bytes is NULL."
    return s.c_str() == nullptr ? [NSString string]
                                : [NSString stringWithUTF8String:s.c_str()];
}
}
