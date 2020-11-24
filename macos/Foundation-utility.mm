#include "Foundation-utility.h"
#include "common-objc.h"

auto resourcePath(const std::string &stem, const std::string &extension)
    -> std::string {
    return [[NSBundle mainBundle] pathForResource:nsString(stem)
                                           ofType:nsString(extension)]
        .UTF8String;
}
