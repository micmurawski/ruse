#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include "core.hpp"
#include "tokenizers.hpp"
#include <vector>
#include <memory>
#include <string>
#include <numeric>
#include <format>
#include <type_traits>
#include <ostream>

template <typename T>
class Analyzer
{
public:
    std::unique_ptr<Tokenizer<T>> tokenizer;
    std::vector<std::unique_ptr<Composable>> items;

    Analyzer() = default;

    Analyzer(std::unique_ptr<Tokenizer<T>> tok) : tokenizer(std::move(tok)), items() {};

    void add_item(std::unique_ptr<Composable> item)
    {
        items.push_back(std::move(item));
    };

    operator string() const
    {
        string s = std::accumulate(begin(this->items), end(this->items), string(),
                                   [](const string &ss, const std::unique_ptr<Composable> &item)
                                   {
                                       return ss.empty() ? item->to_string() : ss + ", " + item->to_string();
                                   });

        string tokenizer_str = this->tokenizer != nullptr
                                   ? string(*dynamic_cast<const T *>(tokenizer.get()))
                                   : "null";

        return std::format("Analyzer(tokenizer={}, items=[{}])", tokenizer_str, s);
    }
};

template <typename T>
ostream &operator<<(std::ostream &os, const Analyzer<T> &analyzer)
{
    os << string(analyzer);
    return os;
};

// Operator overloading for Tokenizer<T> || Composable
template <typename T, typename U>
auto operator||(Tokenizer<T> &&left, U &&right) -> Analyzer<std::remove_reference_t<T>>
{
    static_assert(std::is_base_of_v<Composable, std::remove_reference_t<U>>,
                  "Right type must derive from Composable");

    auto analyzer = Analyzer<T>(std::make_unique<T>(std::move(left)));
    analyzer.add_item(std::make_unique<U>(std::forward<U>(right)));
    return analyzer;
};

template <typename T, typename U>
auto operator||(Analyzer<T> &&left, U &&right) -> Analyzer<std::remove_reference_t<T>>
{
    static_assert(std::is_base_of_v<Composable, std::remove_reference_t<U>>,
                  "Right type must derive from Composable");

    left.add_item(std::make_unique<U>(std::forward<U>(right)));
    return left;
}

#endif // ANALYZER_HPP