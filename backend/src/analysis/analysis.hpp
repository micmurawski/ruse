#pragma once
#include <any>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <regex>

using namespace std;

namespace analysis
{
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

    Tokenizer(TokenizerConfig config)
        : _end(),
          config(config)
    {
      current_token = new Token(config.chars, config.positions, false, config.remove_stops, 1.0, 0);
      if (config.text != nullptr)
      {
        pattern = regex(config.pattern);
        cout << config.pattern << endl;
        current = sregex_iterator(config.text->begin(), config.text->end(), pattern);
      }
      else
        current = sregex_iterator();
      handle_current_token();
    };
    Tokenizer();
    Tokenizer(const Tokenizer &t) : Composable(), current(), _end()
    {
      this->config = TokenizerConfig(t.config);
      this->prev_end = t.prev_end;

      if (t.current_token != nullptr)
        this->current_token = new Token(*t.current_token);

      this->current = sregex_iterator(t.current);
      this->_end = sregex_iterator();
    }
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
    IDTokenizer(TokenizerConfig config) : Tokenizer()
    {
      this->config = config;
      this->config.tokenize = false;

      this->current_token = new Token(config.chars, config.positions, false, config.remove_stops, 1.0, 0);
      if (config.text != nullptr)
      {
        pattern = regex(config.pattern);
        cout << config.pattern << endl;
        current = sregex_iterator(config.text->begin(), config.text->end(), pattern);
      }
      else
        current = sregex_iterator();
      handle_current_token();
    }
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
  CompositeAnalyzer operator||(const L &left, const R &right)
  {
    cout << string(left) << "||" << string(right) << endl;
    CompositeAnalyzer res{};

    if constexpr (is_same<L, CompositeAnalyzer>::value)
    {
      if (left.tokenizer.has_value())
        res.tokenizer = optional<Tokenizer>(left.tokenizer.value());
    }
    else if constexpr (is_same<R, CompositeAnalyzer>::value)
    {
      if (right.tokenizer.has_value())
        res.tokenizer = optional<Tokenizer>(right.tokenizer.value());
    }

    res.add(left);
    res.add(right);
    return res;
  }
}