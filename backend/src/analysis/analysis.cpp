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
    return format(
        "CompositeAnalyzer(tokenizer={}, items=[{}])",
        tokenizer_str,
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

    return format("Token(text=\"{}\", pos={})", text, pos);
  }
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
  // Tokenizer
  Tokenizer::operator string() const
  {
    return format("Tokenizer(pattern=\"{}\", positions={}, chars={}, mode=\"{}\")", config.pattern, config.positions, config.chars, config.mode);
  };

  Tokenizer::Tokenizer() : current(), _end() {};
  Tokenizer &Tokenizer::begin() { return *this; }
  Tokenizer Tokenizer::end() const { return Tokenizer(); };
  bool Tokenizer::operator==(const Tokenizer &other) const { return current == other.current; };
  bool Tokenizer::operator!=(const Tokenizer &other) const { return !(*this == other); };
  void Tokenizer::reset()
  {
    prev_end = 0;
    if (current_token != nullptr)
    {
      delete current_token;
      current_token = nullptr;
    }
  };
  Tokenizer Tokenizer::operator++(int)
  {
    Tokenizer tmp = *this;
    ++(*this);
    return tmp;
  };
  Tokenizer &Tokenizer::operator++()
  {

    if (current != _end)
    {
      current++;
      handle_current_token();
    }

    return *this;
  };

  Token &Tokenizer::operator*() const { return *current_token; };
  Token *Tokenizer::operator->() const { return current_token; };

  void Tokenizer::handle_current_token()
  {
    if (current == _end)
    {
      cout << "ended" << endl;
      this->reset();
      return;
    }

    if (!config.tokenize)
    {
      if (current_token->text == *config.text)
      {
        current = _end;
        return;
      }
      current_token->text = *config.text;
      current_token->original = *config.text;
      if (config.positions)
        current_token->pos = 0;
      if (config.chars)
      {
        current_token->start_char = config.start_char;
        current_token->end_char = config.start_char + static_cast<int>(config.text->length());
      }
    }
    else if (!config.gaps)
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
}