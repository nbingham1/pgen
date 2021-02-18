#pragma once

#include <pgen/grammar.h>

namespace pgen
{

const int32_t CHARACTER = -1;
const int32_t KEYWORD = -2;
const int32_t INSTANCE = -3;
const int32_t TEXT = -4;
const int32_t WHITESPACE = -5;
const int32_t INTEGER = -6;
const int32_t CHARACTER_CLASS = -7; 
const int32_t REGULAR_EXPRESSION = -8; 

struct stem : symbol_t
{
	stem(int index, bool keep = true);
	virtual ~stem();

	int index;

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct character_match : symbol_t
{
	character_match();
	character_match(std::string match, bool keep = true);
	~character_match();

	std::vector<std::pair<char, char> > ranges;
	bool invert;

	std::string escape(char c) const;
	char unescape(char c) const;
	std::string name() const;

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct keyword : symbol_t
{
	keyword(std::string value, bool keep = true);
	~keyword();

	std::string value;

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct instance : symbol_t
{
	instance(bool keep = true);
	~instance();

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct text : symbol_t
{
	text(bool keep = true);
	~text();

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct whitespace : symbol_t
{
	whitespace(bool brk = true, bool keep = false);
	~whitespace();

	bool brk;

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct integer : symbol_t
{
	integer(int base, bool keep = true);
	~integer();

	int base;

	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct character_class : symbol_t
{
	character_class(bool keep = true);
	~character_class();
	
	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct character
{
	character(uint8_t from, uint8_t to);
	~character();

	uint8_t from, to;
	std::vector<int16_t> branch;

	bool match(uint8_t c) const;
};

struct segment
{
	std::string::iterator i;
	std::vector<int16_t> from;
	std::vector<int16_t> to;
};

struct regular_expression : symbol_t
{
	regular_expression(std::string value, bool keep = true);
	~regular_expression();

	std::string value;
	std::vector<int16_t> start;
	std::vector<character> expr;

	char unescape(std::string::iterator &i) const;
	bool load(std::string::iterator begin, std::string::iterator end, segment &result);
	parsing parse(lexer_t &lexer) const;
	symbol_t *clone(int rule_offset = 0) const;
	std::string emit() const;
};

}
