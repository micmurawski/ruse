#include "core.hpp"
#include <format>
#include <string>
#include <memory>
#include <initializer_list>
#include <optional>
#include <iterator>
#include <regex>
#include <vector>
#include <numeric>

using namespace std;
// Token
Token::Token(
    bool chars,
    bool positions,
    bool stopped,
    bool remove_stops,
    float boost,
    int pos,
    int start_char,
    int end_char,
    string mode,
    string text,
    string original)
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
Token::operator string() const { return format("Token(text=\"{}\", pos={}, original=\"{}\")", text, pos, original); };
bool Token::operator==(const Token &another) const
{
    return this->text == another.text && this->pos == another.pos && this->original == another.original;
};
// Composable
Composable::operator string() const { return format("Composable(is_morph={})", is_morph); }
bool Composable::has_morph() { return is_morph; }
