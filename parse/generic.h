#pragma once

#include <vector>
#include <map>

#include <parse/grammar.h>

namespace parse
{

struct generic_t : grammar_t
{
	struct segment
	{
		segment();
		~segment();

		grammar_t::links start, end;
		bool skip;
		std::vector<message> msgs;

		segment &sequence(const segment &s);
		segment &parallel(const segment &s);
	};

	std::map<std::string, int> definitions;
	std::vector<std::string> imports;

	segment load_term(lexer_t &lexer, const token_t &token);
	segment load_sequence(lexer_t &lexer, const token_t &token);
	segment load_choice(lexer_t &lexer, const token_t &token);
	void load_definition(lexer_t &lexer, const token_t &token);
	void load_import(lexer_t &lexer, const token_t &token);
	void load_grammar(lexer_t &lexer, const token_t &token);
	void load(std::string filename);
};

}
