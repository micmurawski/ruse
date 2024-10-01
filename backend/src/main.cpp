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
    TokenizerConfig config = TokenizerConfig();
    string test_string = "AAAaaaBBBbbbCCCcccDDDddd";
    config.pattern = "[A-Z]+";

    // string test_string = "/alfa/bravo/charlie/delta/";
    // config.pattern = "[^/]+";
    config.text = &test_string;
    config.positions = true;
    RegexTokenizer id_tokenizer = RegexTokenizer({.text = &test_string, .positions = true, .pattern = "[A-Z]+"});
    // vector<Token> tokens = vector(id_tokenizer.begin(), id_tokenizer.end());
    //// cout << typeid(id_tokenizer++).name() << endl;
    //
    for (auto &t : id_tokenizer)
    {
        cout << string(t) << endl;
    }
    string test_string_path = "/alfa/bravo/charlie/delta/";
    PathTokenizer path_tokenizer = PathTokenizer({.text = &test_string_path, .positions = true});
    for (auto &t : path_tokenizer)
    {
        cout << string(t) << endl;
    }

    // regex _regex = regex("[^/]+");
    // sregex_iterator current = sregex_iterator(test_string_path.begin(), test_string_path.end(), _regex);
    // sregex_iterator _end = sregex_iterator();
    // for (; current != _end; current++)
    //{
    //     auto match = *current;
    //     cout << match.str() << endl;
    // }
    //  vector<Token> tokens = vector(path_tokenizer.begin(), path_tokenizer.end());
    //  for (;path_tokenizer != path_tokenizer.end(); path_tokenizer++)
    //{
    //      std::cout << std::string(*path_tokenizer) << std::endl;
    //  }

    return 0;
}