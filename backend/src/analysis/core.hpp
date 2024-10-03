#ifndef CORE_HPP
#define CORE_HPP
#pragma once
#include <string>
#include <iterator>
#include <optional>
#include <vector>
#include <format>
#include <iostream>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <utility>

using namespace std;
class Composable
{
public:
    bool is_morph;
    Composable(bool is_morph = false) : is_morph(is_morph) {};
    operator string() const;
    bool has_morph();
};

class Token
{
public:
    bool chars;
    bool positions;
    bool stopped;
    bool remove_stops;
    float boost;
    int pos;
    int start_char;
    int end_char;
    string mode;
    string text;
    string original;
    Token(bool chars = false, bool positions = false, bool stopped = false,
          bool remove_stops = true, float boost = 1.0, int pos = 0,
          int start_char = 0, int end_char = 0, string mode = "",
          string text = "", string original = "");
    Token(string text, int pos);
    Token(const Token &token);
    Token(const char *str, int pos);
    bool operator==(const Token &another) const;
    operator string() const;
};

template <typename Impl>
class Tokenizer
{
protected:
    Token *current_token = nullptr;
    virtual void handle_current_token() {};
    virtual void reset();

public:
    using iterator_category = forward_iterator_tag;
    using value_type = Token;
    using difference_type = ptrdiff_t;
    using pointer = Token *;
    using reference = Token &;

    virtual Impl &operator++();
    virtual Impl operator++(int);
    Token &operator*() const;
    Token *operator->() const;
    Impl &begin();
    virtual Impl end() const;
    bool operator!=(const Impl &other) const;
};

template <typename T>
class CompositeAnalyzer : public Composable
{

public:
    vector<Composable> items;
    optional<Tokenizer<T>> tokenizer;

    CompositeAnalyzer();
    CompositeAnalyzer(initializer_list<Composable> composables,
                      optional<Tokenizer<T>> tokenizer = nullopt);
    bool has_morph();
    void add(const Composable &composable);
    void add(const Tokenizer<T> &_tokenizer);
    void add(const CompositeAnalyzer &composite_analyzer);
    operator string() const;
};

// Tokenizer
template <typename Impl>
Impl &Tokenizer<Impl>::operator++()
{
    handle_current_token();
    Impl &tokenizer = static_cast<Impl &>(*this);
    return tokenizer;
};
template <typename Impl>
Impl Tokenizer<Impl>::operator++(int)
{
    Impl tmp(static_cast<Impl &>(*this));
    operator++();
    return tmp;
};
template <typename Impl>
void Tokenizer<Impl>::reset()
{
    if (current_token != nullptr)
    {
        delete current_token;
        current_token = nullptr;
    }
};
template <typename Impl>
Impl Tokenizer<Impl>::end() const
{
    Impl tokenizer = Impl();
    return tokenizer;
};
template <typename Impl>
bool Tokenizer<Impl>::operator!=(const Impl &other) const { return !(*this == other); }
template <typename Impl>
Token &Tokenizer<Impl>::operator*() const { return *current_token; }
template <typename Impl>
Token *Tokenizer<Impl>::operator->() const { return current_token; };
template <typename Impl>
Impl &Tokenizer<Impl>::begin() { return static_cast<Impl &>(*this); };

template <typename T>
string join(vector<T> const &vec, string delim)
{
    if (vec.empty())
        return string();
    return accumulate(vec.begin() + 1, vec.end(), string(vec[0]), [](const string &a, T b)
                      { return a + ", " + string(b); });
};

template <typename T, typename L, typename R>
CompositeAnalyzer<T> operator||(const L &left, const R &right)
{
    CompositeAnalyzer<T> res{};

    if constexpr (is_same<L, CompositeAnalyzer<T>>::value)
    {
        if (left.tokenizer.has_value())
            res.tokenizer = optional<Tokenizer<T>>(left.tokenizer.value());
    }
    else if constexpr (is_same<R, CompositeAnalyzer<T>>::value)
    {
        if (right.tokenizer.has_value())
            res.tokenizer = optional<Tokenizer<T>>(right.tokenizer.value());
    }

    res.add(left);
    res.add(right);
    return res;
};

template <typename T>
CompositeAnalyzer<T>::operator string() const
{
    string s = accumulate(begin(this->items), end(this->items), string(), [](string ss, string s)
                          { return ss.empty() ? s : ss + ", " + s; });
    string tokenizer_str = this->tokenizer.has_value()
                               ? string(this->tokenizer.value())
                               : string("null");
    return format("CompositeAnalyzer(tokenizer={}, items=[{}])", tokenizer_str, s);
}
template <typename T>
bool CompositeAnalyzer<T>::has_morph()
{
    if (tokenizer.has_value() and tokenizer.value().has_morph())
        return true;
    for (auto &item : items)
        if (item.has_morph())
            return true;
    return false;
};
template <typename T>
void CompositeAnalyzer<T>::add(const Composable &composable)
{
    items.push_back(composable);
}
template <typename T>
void CompositeAnalyzer<T>::add(const Tokenizer<T> &_tokenizer)
{
    if (tokenizer.has_value())
        throw runtime_error("Tokenizer is already assigned");
    tokenizer = optional<Tokenizer<T>>{_tokenizer};
}
template <typename T>
void CompositeAnalyzer<T>::add(const CompositeAnalyzer &composite_analyzer)
{
    for (const Composable &c : composite_analyzer.items)
        this->items.push_back(c);
}

#endif