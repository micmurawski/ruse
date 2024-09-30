#include "analysis/analysis.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <optional>

template <typename Iterator>
std::vector<typename std::iterator_traits<Iterator>::value_type>
make_vector(Iterator begin, Iterator end)
{
    return std::vector<typename std::iterator_traits<Iterator>::value_type>(begin, end);
};
int main()
{
    std::cout << "Hello from the main program!" << std::endl;

    using namespace analysis;
    using namespace std::string_literals;

    TokenizerConfig tokenizer_config = TokenizerConfig();
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd"s;
    tokenizer_config.text = &test_string;
    tokenizer_config.pattern = "[A-Z]+";
    tokenizer_config.positions = true;
    // Tokenizer regex_tokenizer_1 = Tokenizer(tokenizer_config);

    // std::cout << std::string(regex_tokenizer_1) << std::endl;
    // for (auto &t : regex_tokenizer_1)
    //{
    //     std::cout << std::string(t) << std::endl;
    // }
    //  return 0;
    //   std::cout << "FIRST= " << std::string(t1.value()) << std::endl;
    //   std::cout << "LAST= " << std::string(t2.value()) << std::endl;
    Tokenizer regex_tokenizer_2 = Tokenizer(tokenizer_config);

    auto tokens = make_vector(regex_tokenizer_2.begin(), regex_tokenizer_2.end());
    //
    for (auto &t : tokens)
    {
        std::cout << std::string(t) << std::endl;
    }
    IDTokenizer id_tokenizer = IDTokenizer(tokenizer_config);
    for (auto &t : id_tokenizer)
    {
        std::cout << std::string(t) << std::endl;
    }
    vector<Token> expected_tokens{
        Token("AAA", 1),
        Token("BBB", 2),
        Token("CCC", 3),
        Token("DDD", 4),
    };
    // Token t1 = Token("abc", 0);
    // Token t2 = Token("abc", 2);
    bool _eq = std::equal(tokens.begin(), tokens.end(), expected_tokens.begin());
    cout << _eq << endl;
    //  sregex_iterator words_begin = sregex_iterator(test_string.begin(), test_string.end(), regex_1);
    //  sregex_iterator words_end = sregex_iterator();
    //  auto tokens2 = std::vector(words_begin, words_end);
    //
    //  for (const auto &t : tokens2)
    //  {
    //      cout << t.str() << endl;
    //  }
    //  //for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    //  //{
    //  //    smatch match = *i;
    //  //    string match_str = match.str();
    //  //    cout << match_str << '\n';
    //  //}

    return 0;
}