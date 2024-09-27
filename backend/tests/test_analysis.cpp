#include "gtest/gtest.h"
#include "analysis/analysis.hpp"

TEST(AnalysisTest, DummyTest)
{
    // This is just a dummy test
    EXPECT_EQ(1, 1);
}

TEST(ComposableTest, Test1)
{
    using namespace analysis;
    Composable c1 = Composable();
    CompositeAnalyzer c2 = CompositeAnalyzer();
    // CompositeAnalyzer c3 = c1 || c2;
}

#ifdef __APPLE__
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif