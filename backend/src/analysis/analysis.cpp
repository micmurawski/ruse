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

namespace analysis
{
  using namespace std;
  // Composable
  Composable::operator string() const
  {
    return format("Composable(is_morph={})", is_morph);
  }
  bool Composable::has_morph() { return is_morph; }

  // CompositeAnalyzer
  CompositeAnalyzer::CompositeAnalyzer() : Composable() {};
  CompositeAnalyzer::CompositeAnalyzer(initializer_list<Composable> composables,
                                       optional<Tokenizer> tokenizer)
      : Composable(), tokenizer(tokenizer)
  {
    items.insert(items.end(), composables.begin(), composables.end());
  };
  CompositeAnalyzer::operator string() const
  {
    string s = accumulate(
        begin(this->items), end(this->items), string(),
        [](string ss, string s)
        { return ss.empty() ? s : ss + ", " + s; });
    string tokenizer_str = this->tokenizer.has_value()
                               ? string(this->tokenizer.value())
                               : string("null");
    return format("CompositeAnalyzer(tokenizer={}, items=[{}])", tokenizer_str,
                  s);
  }
  bool CompositeAnalyzer::has_morph()
  {
    if (tokenizer.has_value() and tokenizer.value().has_morph())
      return true;
    for (auto &item : items)
    {
      if (item.has_morph())
      {
        return true;
      }
    }
    return false;
  };
  void CompositeAnalyzer::add(const Composable &composable)
  {
    items.push_back(composable);
  }
  void CompositeAnalyzer::add(const Tokenizer &_tokenizer)
  {
    if (tokenizer.has_value())
      throw runtime_error("Tokenizer is already assigned");
    tokenizer = optional<Tokenizer>{_tokenizer};
  }
  void CompositeAnalyzer::add(const CompositeAnalyzer &composite_analyzer)
  {
    for (const Composable &c : composite_analyzer.items)
    {
      this->items.push_back(c);
    }
  }
  // Token
  Token::operator string() const
  {
    {
      return format("Token(text=\"{}\", pos={})", text, pos);
      // return format("Token(text={}, chars={}, positions={}, stopped={},
      // remove_stops={}, boost={:.2f}, pos={}, start_char={}, end_char={},
      // mode={}, original={})", text, chars, positions, stopped, remove_stops,
      // boost, pos, start_char, end_char, mode, original);
    }
  }
  Token::Token(bool chars, bool positions, bool stopped, bool remove_stops,
               float boost, int pos, int start_char, int end_char, string mode,
               string text, string original)
      : chars(chars), positions(positions), stopped(stopped),
        remove_stops(remove_stops), boost(boost), pos(pos),
        start_char(start_char), end_char(end_char), mode(mode), text(text),
        original(original) {};
  // Tokenizer
  Tokenizer::operator string() const
  {
    return format("Tokenizer(positions={}, chars={}, mode={})", positions, chars,
                  mode);
  };
  Tokenizer::Tokenizer(bool positions, bool chars, bool keep_original,
                       bool remove_stops, int start_pos, int start_char,
                       string mode, map<string, any> args)
      : Composable(), positions(positions), chars(chars),
        keep_original(keep_original), remove_stops(remove_stops),
        start_pos(start_pos), start_char(start_char), mode(mode), args(args),
        done(false) {};

  Token &Tokenizer::operator*() const { return *current_token; };
  Token *Tokenizer::operator->() const { return current_token.get(); };
  Tokenizer::value_type Tokenizer::operator()() { return Token(*current_token); };
  Tokenizer &Tokenizer::operator++() { return *this; };
  bool Tokenizer::operator==(const Tokenizer &other) const
  {
    return this->done == other.done;
  };
  bool Tokenizer::operator!=(const Tokenizer &other) const
  {
    return !(*this == other);
  };
  Tokenizer Tokenizer::operator++(int)
  {
    Tokenizer tmp = *this;
    ++(*this);
    return tmp;
  };
  Tokenizer Tokenizer::begin() { return *this; };
  Tokenizer Tokenizer::end()
  {
    Tokenizer end_tokenizer = *this;
    end_tokenizer.done = true;
    return end_tokenizer;
  };

  // ID Tokenizer
  template <typename... Args>
  IDTokenizer::IDTokenizer(string *text, Args... args)
      : Tokenizer(args...), text(text)
  {

    Token _token =
        Token(chars = chars, positions = positions, remove_stops = remove_stops);

    if (text)
    {
      _token.text = *text;
      _token.boost = 1.0;
      if (keep_original)
        _token.original = *text;

      if (positions)
        _token.pos = start_char + 1;
      if (chars)
      {
        _token.start_char = start_char;
        _token.end_char = start_char + size(*text);
      }
    }
    current_token = make_shared<Token>(_token);

    this->done = false;
  };
  IDTokenizer &IDTokenizer::operator++()
  {
    if (!done)
      this->done = true;

    return *this;
  };
  IDTokenizer IDTokenizer::begin() { return *this; };
  IDTokenizer IDTokenizer::end()
  {
    IDTokenizer _end = IDTokenizer();
    _end.done = true;
    return _end;
  };

  // RegexTokenizer

  RegexTokenizer::RegexTokenizer() : Tokenizer(), current(), _end() {};
  RegexTokenizer &RegexTokenizer::begin() { return *this; }
  RegexTokenizer RegexTokenizer::end() const { return RegexTokenizer(); };
  bool RegexTokenizer::operator==(const RegexTokenizer &other) const
  {
    return current == other.current;
  };
  bool RegexTokenizer::operator!=(const RegexTokenizer &other) const
  {
    return !(*this == other);
  };
  void RegexTokenizer::handle_current_token()
  {
    if (current == _end)
      return;
    if (!tokenize)
    {
      current_token.get()->text = *text;
      current_token.get()->original = *text;
      if (positions)
        current_token.get()->pos++;
      if (chars)
      {
        current_token.get()->start_char = start_char;
        current_token.get()->end_char =
            start_char + static_cast<int>(text->length());
      }
    }
    else if (!gaps)
    {
      if (current != _end)
        current_token.get()->text = current->str();

      current_token.get()->boost = 1.0;

      if (keep_original)
        current_token.get()->original = *text;
      current_token.get()->stopped = false;
      if (positions)
        current_token.get()->pos++;

      if (chars)
      {
        current_token.get()->start_char = start_char + current->position();
        current_token.get()->end_char =
            start_char + current->position() + current->length();
      }
    }
    else
    {
      string slice = text->substr(prev_end, current->position() - prev_end);
      if (slice.length())
      {
        current_token.get()->text = slice;
        current_token.get()->boost = 1.0;
        if (keep_original)
          current_token.get()->original = *text;
        current_token.get()->stopped = false;
        if (positions)
          current_token.get()->pos = ++current_token.get()->pos;
        if (chars)
        {
          current_token.get()->start_char = start_char + prev_end;
          current_token.get()->end_char =
              prev_end + current->position() + current->length();
        }
      }
      prev_end = current->position() + current->length();
    }
  };
  RegexTokenizer &RegexTokenizer::operator++()
  {
    if (!tokenize)
    {
      current = _end;
    }
    else
    {
      current++;
    }
    handle_current_token();
    return *this;
  };
}