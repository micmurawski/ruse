#include "gtest/gtest.h"
#include "analysis/tokenizers.hpp"
#include <string>
#include <vector>
#include <format>

using namespace std;
using namespace std::string_literals;

TEST(AnalysisTest, TestRegexTokenizer)
{
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd";
    RegexTokenizer regex_tokenizer = RegexTokenizer({.text = &test_string, .pattern = "[A-Z]+", .positions = true});
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
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd"s;
    IDTokenizer id_tokenizer = IDTokenizer({.text = &test_string});
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
        Token("alfa", 0),
        Token("alfa/bravo", 1),
        Token("alfa/bravo/charlie", 2),
        Token("alfa/bravo/charlie/delta", 3),
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