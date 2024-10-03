#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP
#pragma once
#include "core.hpp"
#include <iterator>
#include <memory>
#include <optional>
#include <regex>
#include <vector>
#include <numeric>
#include <string>

namespace analysis
{
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

  class Analyzer : public Composable
  {
    void clean();
    operator string() const;
  };

  class RegexTokenizer : public Tokenizer<RegexTokenizer>, public Composable
  {
  protected:
    sregex_iterator current;
    sregex_iterator _end;
    regex pattern;
    int prev_end = 0;
    void reset();

  public:
    TokenizerConfig config;

    RegexTokenizer(TokenizerConfig config = TokenizerConfig());
    RegexTokenizer(const RegexTokenizer &t);
    bool operator==(const RegexTokenizer &other) const;
    operator string() const;
    virtual void handle_current_token();
  };

  class IDTokenizer : public Tokenizer<IDTokenizer>, public Composable
  {
  protected:
    bool _end;

  public:
    TokenizerConfig config;
    IDTokenizer(TokenizerConfig config = TokenizerConfig(), bool _end = false);
    IDTokenizer end() const override { return IDTokenizer({}, true); };
    IDTokenizer &operator++() override;
    IDTokenizer operator++(int) override;
    bool operator==(const IDTokenizer &other);
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

    bool operator==(const PathTokenizer &other) const;
    void handle_current_token();
  };
}
#endif