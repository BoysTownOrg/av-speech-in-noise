//
//  assert-utility.h
//  av-coordinated-response-measure
//
//  Created by Bashford, Seth on 3/11/19.
//

#ifndef assert_utility_h
#define assert_utility_h

#include <gtest/gtest.h>

inline void assertEqual(std::string expected, std::string actual) {
    EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(std::vector<T> expected, std::vector<T> actual) {
    EXPECT_EQ(expected.size(), actual.size());
    for (typename std::vector<T>::size_type i{0}; i < expected.size(); ++i)
        EXPECT_EQ(expected.at(i), actual.at(i));
}

#endif /* assert_utility_h */
