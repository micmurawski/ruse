#pragma once
#include <any>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <regex>

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
    Token(const Token &token);
    operator string() const;
  };
  class Composable
  {
  public:
    bool is_morph;
    Composable(bool is_morph = false) : is_morph(is_morph) {};
    operator string() const;
    bool has_morph();
  };

  class Analyzer : public Composable
  {
    void clean();
    operator string() const;
  };

  class Tokenizer : public Composable
  {
  protected:
    regex pattern;
    sregex_iterator current;
    sregex_iterator _end;
    int prev_end = 0;
    Token *current_token = nullptr;
    void reset();

  public:
    TokenizerConfig config;

    using iterator_category = forward_iterator_tag;
    using value_type = Token;
    using difference_type = ptrdiff_t;
    using pointer = Token *;
    using reference = Token &;
    operator string() const;

    Tokenizer(TokenizerConfig config);
    Tokenizer(const Tokenizer &t);
    Tokenizer();
    Tokenizer &begin();
    Tokenizer end() const;

    bool operator==(const Tokenizer &other) const;
    bool operator!=(const Tokenizer &other) const;
    void handle_current_token();
    Tokenizer &operator++();
    Tokenizer operator++(int);
    reference operator*() const;
    pointer operator->() const;
  };

  class IDTokenizer : public Tokenizer
  {
  public:
    IDTokenizer(TokenizerConfig config);
  };

  class CompositeAnalyzer : public Composable
  {

  public:
    vector<Composable> items;
    optional<Tokenizer> tokenizer;

    CompositeAnalyzer();
    CompositeAnalyzer(initializer_list<Composable> composables,
                      optional<Tokenizer> tokenizer = nullopt);
    bool has_morph();
    void add(const Composable &composable);
    void add(const Tokenizer &_tokenizer);
    void add(const CompositeAnalyzer &composite_analyzer);
    operator string() const;
  };

  template <typename L, typename R>
  CompositeAnalyzer operator||(const L &left, const R &right);
}