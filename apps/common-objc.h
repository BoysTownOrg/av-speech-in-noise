#ifndef APPS_COMMON_OBJC_H_
#define APPS_COMMON_OBJC_H_

#import <Foundation/Foundation.h>
#include <string>

inline NSString *asNsString(std::string s) {
    return [NSString
        stringWithCString:s.c_str()
        encoding:NSString.defaultCStringEncoding
    ];
}

#endif
