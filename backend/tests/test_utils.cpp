#include "gtest/gtest.h"
#include "utils/utils.hpp"

TEST(UtilsTest, DummyTest) {
    // This is just a dummy test
    EXPECT_EQ(1, 1);
}

#ifdef __APPLE__
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif 