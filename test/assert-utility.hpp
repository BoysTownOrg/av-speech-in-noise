#ifndef TESTS_ASSERT_UTILITY_HPP_
#define TESTS_ASSERT_UTILITY_HPP_

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <functional>

#define AV_SPEECH_IN_NOISE_EXPECT_TRUE(c) EXPECT_TRUE(c)

#define AV_SPEECH_IN_NOISE_EXPECT_FALSE(c) EXPECT_FALSE(c)

#define AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected, actual)                      \
    EXPECT_EQ(expected, actual)

#define AV_SPEECH_IN_NOISE_ASSERT_EQUAL(expected, actual)                      \
    ASSERT_EQ(expected, actual)

template <typename T> void assertEqual(const T &expected, const T &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected, actual);
}

inline void assertEqual(
    const std::string &expected, const std::string &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected, actual);
}

template <typename T>
void assertEqual(const std::vector<T> &expected, const std::vector<T> &actual) {
    assertEqual(expected.size(), actual.size());
    for (typename std::vector<T>::size_type i{0}; i < expected.size(); ++i)
        assertEqual(expected.at(i), actual.at(i));
}

template <typename T>
static void assertEqual(const std::vector<T> &expected,
    const std::vector<T> &actual,
    const std::function<void(const T &, const T &)> &elementAssertion) {
    assertEqual(expected.size(), actual.size());
    for (typename std::vector<T>::size_type i{0}; i < expected.size(); ++i)
        elementAssertion(expected.at(i), actual.at(i));
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
