#include "gtest/gtest.h"
#include "analysis/analysis.hpp"
#include <string>
#include <vector>

using namespace std;
using namespace analysis;
using namespace std::string_literals;

TEST(AnalysisTest, TestTokenizer)
{
    TokenizerConfig config = TokenizerConfig();
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd"s;
    config.text = &test_string;
    Tokenizer regex_tokenizer = Tokenizer(config);
    // vector<Token> tokens = vector(begin(regex_tokenizer), end(regex_tokenizer));
}

#ifdef __APPLE__
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif