#include "analysis.hpp"
#include <any>
#include <cstddef>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <typeinfo>

using namespace std;
using namespace std::string_literals;

namespace analysis
{

  // Composable
  Composable::operator string() const
  {
    return format("Composable(is_morph={})", is_morph);
  }
  bool Composable::has_morph() { return is_morph; }

  // CompositeAnalyzer
  template <typename T>
  CompositeAnalyzer<T>::CompositeAnalyzer() : Composable(){};
  template <typename T>
  CompositeAnalyzer<T>::CompositeAnalyzer(initializer_list<Composable> composables,
                                          optional<Tokenizer<T>> tokenizer)
      : Composable(), tokenizer(tokenizer)
  {
    items.insert(items.end(), composables.begin(), composables.end());
  };
  template <typename T>
  CompositeAnalyzer<T>::operator string() const
  {
    string s = accumulate(begin(this->items), end(this->items), string(), [](string ss, string s)
                          { return ss.empty() ? s : ss + ", " + s; });
    string tokenizer_str = this->tokenizer.has_value()
                               ? string(this->tokenizer.value())
                               : string("null");
    return format("CompositeAnalyzer(tokenizer={}, items=[{}])", tokenizer_str, s);
  }
  template <typename T>
  bool CompositeAnalyzer<T>::has_morph()
  {
    if (tokenizer.has_value() and tokenizer.value().has_morph())
      return true;
    for (auto &item : items)
      if (item.has_morph())
        return true;
    return false;
  };
  template <typename T>
  void CompositeAnalyzer<T>::add(const Composable &composable)
  {
    items.push_back(composable);
  }
  template <typename T>
  void CompositeAnalyzer<T>::add(const Tokenizer<T> &_tokenizer)
  {
    if (tokenizer.has_value())
      throw runtime_error("Tokenizer is already assigned");
    tokenizer = optional<Tokenizer<T>>{_tokenizer};
  }
  template <typename T>
  void CompositeAnalyzer<T>::add(const CompositeAnalyzer &composite_analyzer)
  {
    for (const Composable &c : composite_analyzer.items)
      this->items.push_back(c);
  }
  // Token
  Token::operator string() const { return format("Token(text=\"{}\", pos={}, original=\"{}\")", text, pos, original); };
  Token::Token(bool chars, bool positions, bool stopped, bool remove_stops,
               float boost, int pos, int start_char, int end_char, string mode,
               string text, string original)
      : chars(chars), positions(positions), stopped(stopped),
        remove_stops(remove_stops), boost(boost), pos(pos),
        start_char(start_char), end_char(end_char), mode(mode), text(text),
        original(original) {};
  Token::Token(const Token &token)
      : chars(token.chars), positions(token.positions), stopped(token.stopped),
        remove_stops(token.remove_stops), boost(token.boost), pos(token.pos),
        start_char(token.start_char), end_char(token.end_char), mode(token.mode), text(token.text),
        original(token.original) {};
  Token::Token(string text, int pos) : pos(pos), text(text) {};
  Token::Token(const char *str, int pos) : pos(pos), text(string(str)) {};
  bool Token::operator==(const Token &another) const
  {
    return this->text == another.text && this->pos == another.pos && this->original == another.original;
  };
  // RegexTokenizer
  RegexTokenizer::RegexTokenizer(TokenizerConfig config) : current(), _end(), config(config)
  {
    if (config.text != nullptr)
    {
      pattern = regex(config.pattern);
      current = sregex_iterator(config.text->begin(), config.text->end(), pattern);
    }
    handle_current_token();
  };
  RegexTokenizer::RegexTokenizer(const RegexTokenizer &t) : Composable(), current(), _end()
  {
    this->config = TokenizerConfig(t.config);
    this->prev_end = t.prev_end;

    if (t.current_token != nullptr)
      this->current_token = new Token(*t.current_token);

    this->current = sregex_iterator(t.current);
    this->_end = sregex_iterator();
  };
  RegexTokenizer::operator string() const
  {
    return format("RegexTokenizer(pattern=\"{}\", positions={}, chars={}, mode=\"{}\")", config.pattern, config.positions, config.chars, config.mode);
  };

  bool RegexTokenizer::operator==(const RegexTokenizer &other) const { return current == other.current; };
  void RegexTokenizer::reset()
  {
    prev_end = 0;
    if (current_token != nullptr)
    {
      delete current_token;
      current_token = nullptr;
    }
  };

  void RegexTokenizer::handle_current_token()
  {
    if (current == _end)
    {
      this->reset();
      return;
    }
    else if (Tokenizer::current_token == nullptr)
    {
      Tokenizer::current_token = new Token(config.chars, config.positions, false, config.remove_stops, 1.0, 0);
      if (config.positions)
        Tokenizer::current_token->pos = -1;

      if (!config.tokenize)
      {
        if (current_token->text == *config.text)
        {
          current = _end;
          return;
        }
        current_token->text = *config.text;
        if (config.keep_original)
          current_token->original = *config.text;
        if (config.positions)
          current_token->pos = 0;
        if (config.chars)
        {
          current_token->start_char = config.start_char;
          current_token->end_char = config.start_char + static_cast<int>(config.text->length());
        }
      }
    }
    else
      current++;

    if (!config.gaps)
    {
      if (current != _end)
      {
        current_token->text = current->str();
        current_token->boost = 1.0;
        if (config.keep_original)
          current_token->original = *config.text;
        current_token->stopped = false;
        if (config.positions)
          current_token->pos++;
        if (config.chars)
        {
          current_token->start_char = config.start_char + current->position();
          current_token->end_char = config.start_char + current->position() + current->length();
        }
      }
    }
    else
    {
      string slice = config.text->substr(prev_end, current->position() - prev_end);
      if (slice.length())
      {
        current_token->text = slice;
        current_token->boost = 1.0;
        if (config.keep_original)
          current_token->original = *config.text;
        current_token->stopped = false;
        if (config.positions)
          current_token->pos = ++current_token->pos;
        if (config.chars)
        {
          current_token->start_char = config.start_char + prev_end;
          current_token->end_char =
              prev_end + current->position() + current->length();
        }
      }
      prev_end = current->position() + current->length();
    }
  };

  template <typename T, typename L, typename R>
  CompositeAnalyzer<T> operator||(const L &left, const R &right)
  {
    CompositeAnalyzer<T> res{};

    if constexpr (is_same<L, CompositeAnalyzer<T>>::value)
    {
      if (left.tokenizer.has_value())
        res.tokenizer = optional<Tokenizer<T>>(left.tokenizer.value());
    }
    else if constexpr (is_same<R, CompositeAnalyzer<T>>::value)
    {
      if (right.tokenizer.has_value())
        res.tokenizer = optional<Tokenizer<T>>(right.tokenizer.value());
    }

    res.add(left);
    res.add(right);
    return res;
  };
}

// bool operator==(const Token &left, const Token &right) { return left.text == right.text; };
// bool operator==(const vector<Token> &left, const vector<Token> &right)
//{
//   return std::equal(left.begin(), left.end(), right.begin());
// };
