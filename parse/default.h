#pragma once

#include <parse/grammar.h>

namespace parse
{

struct stem : grammar_t::symbol
{
	stem(int index, bool keep = true);
	virtual ~stem();

	int index;

	parsing parse(lexer_t &lexer) const;
};

struct character : grammar_t::symbol
{
	character(std::string match);
	~character();

	std::vector<std::pair<char, char> > ranges;
	bool invert;

	std::string escape(char c) const;
	char unescape(char c) const;
	std::string name() const;

	parsing parse(lexer_t &lexer) const;
};

struct keyword : grammar_t::symbol
{
	keyword(std::string value);
	~keyword();

	std::string value;

	parsing parse(lexer_t &lexer) const;
};

struct instance : grammar_t::symbol
{
	instance();
	~instance();

	parsing parse(lexer_t &lexer) const;
};

struct text : grammar_t::symbol
{
	text();
	~text();

	parsing parse(lexer_t &lexer) const;
};

struct whitespace : grammar_t::symbol
{
	whitespace();
	~whitespace();

	parsing parse(lexer_t &lexer) const;
};

struct integer : grammar_t::symbol
{
	integer();
	~integer();

	parsing parse(lexer_t &lexer) const;
};

}
