#include "assert-utility.h"
#include <gtest/gtest.h>

TEST(LearningGetEnvTests, homePath) {
    assertEqual("/Users/basset", std::getenv("HOME"));
}
