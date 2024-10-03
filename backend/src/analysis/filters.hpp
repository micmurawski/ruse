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

regex recompile(string pattern, bool verbose = false, regex::flag_type flags = regex::ECMAScript)
{
    regex::flag_type cpp_flags = regex::ECMAScript;
    cpp_flags = cpp_flags | flags;

    // if (verbose)
    //    return regex(pattern, regex::extended);

    return regex(pattern, cpp_flags);
}

const regex url_regex_pattern = recompile("([A-Za-z+]+://\\S+?(?=\\s|[.]\\s|$|[.]$))|(\\w+([:.]?\\w+)*)", true);

template <typename T>
class Filter : virtual TokenIterator<Filter<T>>, public Composable
{
protected:
    TokenIterator<T> *token_iterator = nullptr;
    Filter(const TokenIterator<T> token_iterator);
};

#endif