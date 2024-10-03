#include "gtest/gtest.h"
#include "analysis/filters.hpp"
#ifdef __APPLE__
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif 