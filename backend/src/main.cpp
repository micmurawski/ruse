#include "analysis/core.hpp"
#include "analysis/tokenizers.hpp"
#include "analysis/filters.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <optional>
#include "analysis/filters.hpp"
#include "analysis/analyzer.hpp"

template <typename Iterator>
std::vector<typename std::iterator_traits<Iterator>::value_type>
make_vector(Iterator begin, Iterator end)
{
    return std::vector<typename std::iterator_traits<Iterator>::value_type>(begin, end);
};
int main()
{
    std::cout << "Hello from the main program!" << std::endl;

    string test_string = "/alfa/bravo/charlie/delta/";
    cout << PassFilter() << endl;
    auto analyzer1 = IDTokenizer() || PassFilter() || XFilter();
    cout << string(analyzer1) << endl;
    // PathTokenizer path_tokenizer = PathTokenizer({.text = &test_string, .positions = true});
    // vector<Token> tokens = vector(begin(path_tokenizer), end(path_tokenizer));
    // //vector<Token> expected_tokens{
    // //    Token("/alfa/bravo/charlie/delta/", 0),
    // //    Token("bravo/charlie/delta/", 1),
    // //    Token("charlie/delta/", 2),
    // //    Token("delta/", 3),
    // //};
    //
    // for (auto &t : tokens)
    //     cout << string(t) << endl;
    //
    // for (auto &t : expected_tokens)
    //    cout << string(t) << endl;

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