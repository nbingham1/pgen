#pragma once

#include <vector>
#include <map>

#include <parse/grammar.h>
#include <parse/peg.h>

namespace parse
{

struct segment_t
{
	segment_t();
	~segment_t();

	std::vector<symbol_t*> start, end;
	bool skip;
	std::vector<message> msgs;

	segment_t &sequence(const segment_t &s);
	segment_t &parallel(const segment_t &s);
};

struct generic_t : peg_t
{
	generic_t();
	~generic_t();

	grammar_t peg;
	std::map<std::string, int> definitions;
	std::vector<std::string> imports;

	segment_t load_term(lexer_t &lexer, const token_t &token, grammar_t &grammar);
	segment_t load_sequence(lexer_t &lexer, const token_t &token, grammar_t &grammar);
	segment_t load_choice(lexer_t &lexer, const token_t &token, grammar_t &grammar);
	void load_definition(lexer_t &lexer, const token_t &token, grammar_t &grammar);
	void load_import(lexer_t &lexer, const token_t &token, grammar_t &grammar);
	void load_grammar(lexer_t &lexer, const token_t &token, grammar_t &grammar);
	void load(std::string filename, grammar_t &grammar);
};

void export_grammar(const grammar_t &gram, std::string space, std::string name, std::ostream &header = std::cout, std::ostream &source = std::cout);

}
