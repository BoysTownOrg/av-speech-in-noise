#ifndef TESTS_ASSERT_UTILITY_H_
#define TESTS_ASSERT_UTILITY_H_

#include <gtest/gtest.h>
#include <string>
#include <vector>

inline void assertTrue(bool c) { EXPECT_TRUE(c); }

inline void assertFalse(bool c) { EXPECT_FALSE(c); }

inline void assertEqual(
    const std::string &expected, const std::string &actual) {
    EXPECT_EQ(expected, actual);
}

template <typename T> void assertEqual(T expected, T actual) {
    EXPECT_EQ(expected, actual);
}

template <typename T>
void assertEqual(const std::vector<T> &expected, const std::vector<T> &actual) {
    assertEqual(expected.size(), actual.size());
    for (typename std::vector<T>::size_type i{0}; i < expected.size(); ++i)
        assertEqual(expected.at(i), actual.at(i));
}

template <typename T> void assertEqual(T expected, T actual, T tolerance) {
    EXPECT_NEAR(expected, actual, tolerance);
}

template <typename T>
void assertEqual(
    const std::vector<T> &expected, const std::vector<T> &actual, T tolerance) {
    assertEqual(expected.size(), actual.size());
    for (typename std::vector<T>::size_type i{0}; i < expected.size(); ++i)
        assertEqual(expected.at(i), actual.at(i), tolerance);
}

#endif
