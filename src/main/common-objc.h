#ifndef common_objc_h
#define common_objc_h

#import <Foundation/Foundation.h>
#include <string>

inline NSString *asNsString(std::string s) {
    return [NSString
        stringWithCString:s.c_str()
        encoding:NSString.defaultCStringEncoding
    ];
}

#endif
