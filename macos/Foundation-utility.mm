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
    return [NSString stringWithCString:s.c_str()
                              encoding:NSString.defaultCStringEncoding];
}
}
