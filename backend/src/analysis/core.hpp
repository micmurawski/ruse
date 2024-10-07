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
    virtual ~Composable() = default;
    bool is_morph;
    Composable(bool is_morph = false) : is_morph(is_morph) {};
    virtual string to_string() const { return format("Composable(is_morph={})", is_morph); };
    virtual operator string() const { return to_string(); };
    bool has_morph();
    friend ostream &operator<<(std::ostream &os, const Composable &comp);
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

    virtual ~Tokenizer() = default;
    virtual Impl &operator++();
    virtual Impl operator++(int);
    virtual Impl *clone() const = 0;
    Token &operator*() const;
    Token *operator->() const;
    Impl &begin();
    virtual Impl end() const;
    bool operator!=(const Impl &other) const;
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
template <typename Impl>
Impl *Tokenizer<Impl>::clone() const { return new Impl(static_cast<Impl &>(*this)); };

template <typename T>
string join(vector<T> const &vec, string delim = ", ")
{
    if (vec.empty())
        return string();
    return accumulate(vec.begin() + 1, vec.end(), string(vec[0]), [delim](const string &a, T b)
                      { return a + delim + string(b); });
};

#endif