#pragma once
#include <any>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
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

  template <typename Impl>
  class Tokenizer
  {

  protected:
    Token *current_token = nullptr;
    virtual void handle_current_token()
    {
      cout << "old nxt" << endl;
      return;
    };
    virtual void reset()
    {
      if (current_token != nullptr)
      {
        delete current_token;
        current_token = nullptr;
      }
    }

  public:
    using iterator_category = forward_iterator_tag;
    using value_type = Token;
    using difference_type = ptrdiff_t;
    using pointer = Token *;
    using reference = Token &;

    virtual Impl &operator++()
    {
      handle_current_token();
      Impl &tokenizer = static_cast<Impl &>(*this);
      return tokenizer;
    };

    virtual Impl operator++(int)
    {
      Impl tmp(static_cast<Impl &>(*this));
      operator++();
      return tmp;
    };
    Token &operator*() const { return *current_token; };
    Token *operator->() const { return current_token; };
    Impl &begin() { return static_cast<Impl &>(*this); };
    virtual Impl end() const
    {
      Impl tokenizer = Impl();
      cout << "end generated" << endl;
      return tokenizer;
    };
    bool operator!=(const Impl &other) const
    {
      cout << "end" << endl;
      bool _val = !(*this == other);
      cout << _val << endl;
      return _val;
    };
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
    IDTokenizer(TokenizerConfig config = TokenizerConfig(), bool _end = false) : _end(_end), config(config)
    {
      current_token = new Token(config.chars, config.positions, true, config.remove_stops, 1.0, 0);
    };
    IDTokenizer end() const override { return IDTokenizer({}, true); };
    IDTokenizer &operator++() override
    {
      if (_end)
        return *this;

      if (config.text)
        current_token->text = *config.text;
      _end = true;

      return *this;
    };
    IDTokenizer operator++(int) override
    {
      IDTokenizer tmp(*this);
      operator++();
      return tmp;
    };
    bool operator==(const IDTokenizer &other) { return this->_end == other._end; }
  };
  class PathTokenizer : public Tokenizer<PathTokenizer>
  {
  protected:
    regex pattern;
    sregex_iterator current;
    sregex_iterator _end;
    int prev_end = 0;

  public:
    TokenizerConfig config;
    PathTokenizer(TokenizerConfig config = TokenizerConfig()) : current(), _end(), config(config)
    {
      config.pattern = "[^/]+";

      if (config.text != nullptr)
      {
        pattern = regex(config.pattern);
        current = sregex_iterator(config.text->begin(), config.text->end(), pattern);
      }
      handle_current_token();
    };

    bool operator==(const PathTokenizer &other) const { return current == other.current; };
    void handle_current_token() override
    {

      if (current == _end)
      {
        reset();
        return;
      }

      if (current_token == nullptr)
      {
        current_token = new Token(config.chars, config.positions, false, config.remove_stops, 1.0, 0);
        if (config.text != nullptr)
        {
          int __end = current->position() + config.text->length();
          cout << config.text->substr(0, __end) << endl;
          current_token->text = config.text->substr(0, __end);
          return;
        }
      }
      else
      {
        current++;
        if (current != _end)
        {
          current_token->text = config.text->substr(current->position(), config.text->length());
          if (config.positions)
            current_token->pos++;
          if (config.keep_original)
            current_token->original = *config.text;
        }
      }
    }
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

  template <typename T, typename L, typename R>
  CompositeAnalyzer<T> operator||(const L &left, const R &right);

  template <typename T>
  string join(vector<T> const &vec, string delim)
  {
    if (vec.empty())
      return string();

    return accumulate(vec.begin() + 1, vec.end(), string(vec[0]), [](const string &a, T b)
                      { return a + ", " + string(b); });
  };
}