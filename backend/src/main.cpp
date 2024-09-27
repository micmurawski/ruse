#include "analysis/analysis.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <string>
#include <regex>

int main()
{
    std::cout << "Hello from the main program!" << std::endl;

    using namespace analysis;

    Tokenizer t1 = {};
    Composable c1 = {};
    CompositeAnalyzer c2 = {};
    CompositeAnalyzer c3 = {};

    CompositeAnalyzer c0 = c1 || c2 || c3 || t1;

    std::cout << std::string(c0) << std::endl;
    std::cout << std::string(c0.tokenizer.value()) << std::endl;

    CompositeAnalyzer c00 = c1 || c2 || c3;

    std::cout << c00.tokenizer.has_value() << std::endl;
    std::string text = "This is example text";
    IDTokenizer id_tokenizer = IDTokenizer(&text);
    std::cout << std::string(id_tokenizer) << std::endl;
    for (auto &t : id_tokenizer)
    {
        std::cout << std::string(t) << std::endl;
    }

    text = "Hello, world! This is a test.";
    std::regex word_regex("\\w+");
    RegexTokenizer regex_tokenizer = RegexTokenizer(word_regex, &text);
    std::cout << std::string(regex_tokenizer) << std::endl;

    for (auto &t : regex_tokenizer)
    {
        std::cout << std::string(t) << std::endl;
    }
    std::cout << "Where is error???" << std::endl;
    std::cout << "Do not tokenize" << std::endl;
    // regex_tokenizer;
    regex_tokenizer = RegexTokenizer(word_regex, &text, false);
    regex_tokenizer.positions = true;
    std::cout << std::string(regex_tokenizer) << std::endl;

    for (auto &t : regex_tokenizer)
    {
        std::cout << std::string(t) << std::endl;
    }

    std::cout << "GAPS " << text << std::endl;
    // regex_tokenizer;

    regex_tokenizer = RegexTokenizer(word_regex, &text, true, true);
    std::cout << std::string(regex_tokenizer) << std::endl;

    for (auto &t : regex_tokenizer)
    {
        std::cout << std::string(t) << std::endl;
    }

    return 0;
}