#pragma once
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
    Token &operator*() const { return *current_token; };
    Token *operator->() const { return current_token; };
    Impl &begin() { return static_cast<Impl &>(*this); };
    virtual Impl end() const;
    bool operator!=(const Impl &other) const;
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
    int prev_end = 0;

  public:
    TokenizerConfig config;
    PathTokenizer(TokenizerConfig config = TokenizerConfig());
    PathTokenizer(const PathTokenizer &t);

    bool operator==(const PathTokenizer &other) const;
    void handle_current_token();
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