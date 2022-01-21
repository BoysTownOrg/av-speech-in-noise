#include "Foundation-utility.h"

namespace av_speech_in_noise {
auto string(NSString *s) -> std::string {
    // https://stackoverflow.com/a/44153992
    return {
        [s UTF8String], [s lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
}

auto resourceUrl(const std::string &stem, const std::string &extension)
    -> LocalUrl {
    const auto resourcePath{
        [[NSBundle mainBundle] pathForResource:nsString(stem)
                                        ofType:nsString(extension)]};
    return LocalUrl{string(resourcePath)};
}

auto nsString(const std::string &s) -> NSString * {
    // https://developer.apple.com/documentation/foundation/nsstring/1497379-stringwithutf8string?language=objc
    // "Raises an exception if bytes is NULL."
    return s.c_str() == nullptr ? [NSString string]
                                : [NSString stringWithUTF8String:s.c_str()];
}
}
