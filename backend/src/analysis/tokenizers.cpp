#include "tokenizers.hpp"
#include <format>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace std::string_literals;

// IDTokenizer
// Constructor
IDTokenizer::IDTokenizer(TokenizerConfig config, bool _end) : _end(_end), config(config)
{
  current_token = new Token(config.chars, config.positions, true, config.remove_stops, 1.0, 0);
};
// Copy Constructor
IDTokenizer::IDTokenizer(const IDTokenizer &tokenizer) : _end(tokenizer._end), config(tokenizer.config)
{
  current_token = new Token(tokenizer.current_token);
};
// Move Constructor
IDTokenizer::IDTokenizer(IDTokenizer &&other) noexcept
    : Tokenizer<IDTokenizer>(std::move(other)), Composable(std::move(other)),
      _end(other._end), config(std::move(other.config)) {}
// Copy assignment operator
IDTokenizer &IDTokenizer::operator=(const IDTokenizer &other)
{
  if (this != &other)
  {
    Tokenizer<IDTokenizer>::operator=(other);
    Composable::operator=(other);
    _end = other._end;
    config = other.config;
  }
  return *this;
};
// Move assignment operator
IDTokenizer &IDTokenizer::operator=(IDTokenizer &&other) noexcept
{
  if (this != &other)
  {
    Tokenizer<IDTokenizer>::operator=(std::move(other));
    Composable::operator=(std::move(other));
    _end = other._end;
    config = std::move(other.config);
  }
  return *this;
}
IDTokenizer &IDTokenizer::operator++()

{
  if (_end)
    return *this;

  if (config.text)
    current_token->text = *config.text;
  _end = true;

  return *this;
};
IDTokenizer IDTokenizer::operator++(int)
{
  IDTokenizer tmp(*this);
  operator++();
  return tmp;
};
bool IDTokenizer::operator==(const IDTokenizer &other) { return this->_end == other._end; };
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
string RegexTokenizer::to_string() const
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
// PathTokenizer
PathTokenizer::PathTokenizer(TokenizerConfig config) : current(), _end(), config(config)
{
  config.pattern = "[^/]+";

  if (config.text != nullptr)
  {
    pattern = regex(config.pattern);
    current = sregex_iterator(config.text->begin(), config.text->end(), pattern);
  }
  handle_current_token();
};
PathTokenizer::PathTokenizer(const PathTokenizer &t) : Composable(), current(), _end()
{
  this->config = TokenizerConfig(t.config);

  if (t.current_token != nullptr)
    this->current_token = new Token(*t.current_token);

  this->current = sregex_iterator(t.current);
  this->_end = sregex_iterator();
  this->start = t.start;
};
bool PathTokenizer::operator==(const PathTokenizer &other) const { return current == other.current; };
void PathTokenizer::handle_current_token()
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
      start = current->position();
      current_token->text = config.text->substr(start, current->position() + current->length() - 1);
      return;
    }
  }
  else
  {
    current++;
    if (current != _end)
    {
      current_token->text = config.text->substr(start, current->position() + current->length() - 1);
      if (config.positions)
        current_token->pos++;
      if (config.keep_original)
        current_token->original = *config.text;
    }
  }
}

// bool operator==(const Token &left, const Token &right) { return left.text == right.text; };
// bool operator==(const vector<Token> &left, const vector<Token> &right)
//{
//   return std::equal(left.begin(), left.end(), right.begin());
// };
