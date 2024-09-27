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
  public:
    bool positions;
    bool chars;
    bool keep_original;
    bool remove_stops;
    int start_pos;
    int start_char;
    string mode;
    map<string, any> args;
    bool done;
    shared_ptr<Token> current_token;

    using iterator_category = forward_iterator_tag;
    using value_type = string;
    using difference_type = ptrdiff_t;
    using pointer = Token *;
    using reference = Token &;
    operator string() const;

    Tokenizer(bool positions = false, bool chars = false,
              bool keep_original = false, bool remove_stops = true,
              int start_pos = 0, int start_char = 0, string mode = "",
              map<string, any> args = map<string, any>());

    reference operator*() const;
    pointer operator->() const;
    value_type operator()();
    Tokenizer &operator++();
    bool operator==(const Tokenizer &other) const;
    bool operator!=(const Tokenizer &other) const;
    Tokenizer operator++(int);
    Tokenizer begin();
    Tokenizer end();
  };

  class RegexTokenizer : public Tokenizer
  {
  private:
    string *text = nullptr;
    sregex_iterator current;
    sregex_iterator _end;
    int prev_end = 0;

  public:
    bool tokenize;
    bool gaps;
    template <typename... Args>

    RegexTokenizer(regex pattern = regex("[\\w\\*]+(\\.?[\\w\\*]+)*"),
                   string *text = nullptr, bool tokenize = true,
                   bool gaps = false, Args... args)
        : Tokenizer(args...), text(text),
          current(text->begin(), text->end(), pattern), _end(),
          tokenize(tokenize), gaps(gaps)
    {
      Token _token = Token(chars = chars, positions = positions,
                           remove_stops = remove_stops);
      current_token = make_shared<Token>(_token);
      handle_current_token();
    };
    RegexTokenizer();
    RegexTokenizer &begin();
    RegexTokenizer end() const;

    bool operator==(const RegexTokenizer &other) const;
    bool operator!=(const RegexTokenizer &other) const;
    void handle_current_token();
    RegexTokenizer &operator++();
  };

  class IDTokenizer : public Tokenizer
  {
  public:
    string *text = nullptr;
    template <typename... Args>
    IDTokenizer(string *text = nullptr, Args... args);
    IDTokenizer &operator++();
    IDTokenizer begin();
    IDTokenizer end();
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