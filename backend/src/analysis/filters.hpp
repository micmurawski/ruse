#ifndef FILTERS_HPP
#define FILTERS_HPP
#pragma once
#include "core.hpp"
#include <unordered_set>
#include <string>
#include <regex>
#include <optional>
#include <iostream>

using namespace std;

const unordered_set<string> STOP_WORDS = {
    "a",
    "an",
    "and",
    "are",
    "as",
    "at",
    "be",
    "by",
    "can",
    "for",
    "from",
    "have",
    "if",
    "in",
    "is",
    "it",
    "may",
    "not",
    "of",
    "on",
    "or",
    "tbd",
    "that",
    "the",
    "this",
    "to",
    "us",
    "we",
    "when",
    "will",
    "with",
    "yet",
    "you",
    "your"};

regex recompile(string pattern, regex::flag_type flags = regex::ECMAScript)
{
    regex::flag_type cpp_flags = regex::ECMAScript;
    cpp_flags = cpp_flags | flags;
    return regex(pattern, cpp_flags);
}

const regex url_regex_pattern = recompile("([A-Za-z+]+://\\S+?(?=\\s|[.]\\s|$|[.]$))|(\\w+([:.]?\\w+)*)");

class PassFilter : public Composable
{
public:
    ~PassFilter() = default;
    PassFilter() : Composable() {};
    PassFilter(const PassFilter &other) : Composable(other) {};
    PassFilter(PassFilter &&other) noexcept : Composable(std::move(other)) {};
    PassFilter &operator=(const PassFilter &other)
    {
        if (this != &other)
        {
            Composable::operator=(other);
        }
        return *this;
    };
    PassFilter &operator=(PassFilter &&other) noexcept
    {
        if (this != &other)
        {
            Composable::operator=(std::move(other));
        }
        return *this;
    };

    virtual bool apply(const Token &token) { return true; };
    virtual operator string() const override { return to_string(); };
    virtual string to_string() const override { return "PassFilter()"; }
};

class XFilter : public PassFilter
{

    string to_string() const override { return "XFilter()"; }
};

#endif