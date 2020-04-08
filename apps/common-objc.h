#ifndef APPS_COMMON_OBJC_H_
#define APPS_COMMON_OBJC_H_

#import <Foundation/Foundation.h>
#include <string>

inline auto asNsString(const std::string &s) -> NSString * {
    return [NSString stringWithCString:s.c_str()
                              encoding:NSString.defaultCStringEncoding];
}

#endif
