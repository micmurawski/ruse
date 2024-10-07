#ifndef TOKENIZERS_HPP
#define TOKENIZERS_HPP
#pragma once
#include "core.hpp"
#include <iterator>
#include <memory>
#include <optional>
#include <regex>
#include <vector>
#include <numeric>
#include <string>

using namespace std;
using namespace string_literals;

struct TokenizerConfig
{
  string *text = nullptr;
  string pattern = "[\\w\\*]+(\\.?[\\w\\*]+)*"s;
  bool tokenize = true;
  bool gaps = false;
  bool positions = false;
  bool chars = false;
  bool keep_original = false;
  bool remove_stops = true;
  int start_pos = 0;
  int start_char = 0;
  string mode = "";
};

class RegexTokenizer : public Tokenizer<RegexTokenizer>, public Composable
{
protected:
  sregex_iterator current;
  sregex_iterator _end;
  regex pattern;
  int prev_end = 0;
  void reset() override;

public:
  TokenizerConfig config;

  RegexTokenizer(TokenizerConfig config = TokenizerConfig());
  RegexTokenizer(const RegexTokenizer &t);
  ~RegexTokenizer() { delete current_token; }
  RegexTokenizer *clone() const override { return new RegexTokenizer(*this); };
  bool operator==(const RegexTokenizer &other) const;
  string to_string() const override;
  virtual void handle_current_token() override;
};

class IDTokenizer : public Tokenizer<IDTokenizer>, public Composable
{
protected:
  bool _end;

public:
  TokenizerConfig config;
  IDTokenizer(TokenizerConfig config = TokenizerConfig(), bool _end = false);
  IDTokenizer(const IDTokenizer &tokenizer);
  IDTokenizer(IDTokenizer &&other) noexcept;
  IDTokenizer end() const override { return IDTokenizer({}, true); };
  IDTokenizer *clone() const override { return new IDTokenizer(*this); };
  IDTokenizer &operator++() override;
  IDTokenizer operator++(int) override;
  IDTokenizer &operator=(const IDTokenizer &other);
  string to_string() const override { return "IDTokenizer()"; };
  IDTokenizer &operator=(IDTokenizer &&other) noexcept;
  bool operator==(const IDTokenizer &other);

  explicit IDTokenizer(const Tokenizer<IDTokenizer> &other)
      : Tokenizer<IDTokenizer>(other), Composable(),
        _end(false), config()
  {
    if (auto *derived = dynamic_cast<const IDTokenizer *>(&other))
    {
      _end = derived->_end;
      config = derived->config;
    }
  };
};
class PathTokenizer : public Tokenizer<PathTokenizer>, public Composable
{
protected:
  regex pattern;
  sregex_iterator current;
  sregex_iterator _end;
  int start;

public:
  TokenizerConfig config;
  PathTokenizer(TokenizerConfig config = TokenizerConfig());
  PathTokenizer(const PathTokenizer &t);
  PathTokenizer *clone() const override { return new PathTokenizer(*this); };

  bool operator==(const PathTokenizer &other) const;
  void handle_current_token() override;
};

#endif