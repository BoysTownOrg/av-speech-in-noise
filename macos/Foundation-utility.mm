#include "Foundation-utility.h"

auto resourcePath(const std::string &stem, const std::string &extension)
    -> std::string {
    return [[NSBundle mainBundle] pathForResource:nsString(stem)
                                           ofType:nsString(extension)]
        .UTF8String;
}

auto nsString(const std::string &s) -> NSString * {
    return [NSString stringWithCString:s.c_str()
                              encoding:NSString.defaultCStringEncoding];
}
