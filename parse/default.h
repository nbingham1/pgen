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
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct character : grammar_t::symbol
{
	character();
	character(std::string match, bool keep = true);
	~character();

	std::vector<std::pair<char, char> > ranges;
	bool invert;

	std::string escape(char c) const;
	char unescape(char c) const;
	std::string name() const;

	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct keyword : grammar_t::symbol
{
	keyword(std::string value, bool keep = true);
	~keyword();

	std::string value;

	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct instance : grammar_t::symbol
{
	instance(bool keep = true);
	~instance();

	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct text : grammar_t::symbol
{
	text(bool keep = true);
	~text();

	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct whitespace : grammar_t::symbol
{
	whitespace(bool brk = true, bool keep = false);
	~whitespace();

	bool brk;

	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct integer : grammar_t::symbol
{
	integer(int base, bool keep = true);
	~integer();

	int base;

	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

struct character_class : grammar_t::symbol
{
	character_class(bool keep = true);
	~character_class();
	
	parsing parse(lexer_t &lexer) const;
	symbol *clone(int rule_offset = 0) const;
	std::string emit() const;
};

}
