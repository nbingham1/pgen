#pragma once

#include <pgen/grammar.h>

namespace pgen
{

const int32_t CHARACTER = -1;
const int32_t REGULAR_EXPRESSION = -2; 

struct stem : symbol_t
{
	stem(int index, bool keep = true);
	virtual ~stem();

	int index;

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
