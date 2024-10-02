#include "gtest/gtest.h"
#include "analysis/analysis.hpp"
#include <string>
#include <vector>
#include <format>

using namespace std;
using namespace analysis;
using namespace std::string_literals;

TEST(AnalysisTest, TestRegexTokenizer)
{
    TokenizerConfig config = TokenizerConfig();
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd"s;
    config.text = &test_string;
    config.pattern = "[A-Z]+";
    config.positions = true;
    RegexTokenizer regex_tokenizer = RegexTokenizer(config);
    vector<Token> tokens = vector(begin(regex_tokenizer), end(regex_tokenizer));
    vector<Token> expected_tokens{
        Token("AAA", 0),
        Token("BBB", 1),
        Token("CCC", 2),
        Token("DDD", 3),
    };
    string expected_tokens_str = format("[{}]", join(expected_tokens, ", "));
    string tokens_str = format("[{}]", join(tokens, ", "));
    EXPECT_EQ(tokens, expected_tokens) << format("failure: {} != {}", tokens_str, expected_tokens_str);
}

TEST(AnalysisTest, TestIDTokenizer)
{
    TokenizerConfig config = TokenizerConfig();
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd"s;
    config.text = &test_string;
    IDTokenizer id_tokenizer = IDTokenizer(config);
    vector<Token> tokens = vector(begin(id_tokenizer), end(id_tokenizer));
    vector<Token> expected_tokens{
        Token("AAAaaaBBBbbbCCCcccDDDddd", 0),
    };
    string expected_tokens_str = format("[{}]", join(expected_tokens, ", "));
    string tokens_str = format("[{}]", join(tokens, ", "));
    EXPECT_EQ(tokens, expected_tokens) << format("failure: {} != {}", tokens_str, expected_tokens_str);
}

TEST(AnalysisTest, TestPathTokenizer)
{
    string test_string = "/alfa/bravo/charlie/delta/";
    PathTokenizer path_tokenizer = PathTokenizer({.text = &test_string, .positions = true});
    vector<Token> tokens = vector(begin(path_tokenizer), end(path_tokenizer));
    vector<Token> expected_tokens{
        Token("/alfa/bravo/charlie/delta/", 0),
        Token("bravo/charlie/delta/", 1),
        Token("charlie/delta/", 2),
        Token("delta/", 3),
    };
    string expected_tokens_str = format("[{}]", join(expected_tokens, ", "));
    string tokens_str = format("[{}]", join(tokens, ", "));
    EXPECT_EQ(tokens, expected_tokens) << format("failure: {} != {}", tokens_str, expected_tokens_str);
}

#ifdef __APPLE__
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif