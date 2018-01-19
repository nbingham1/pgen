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

	segment load_term(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token);
	segment load_sequence(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token);
	segment load_choice(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token);
	void load_definition(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token);
	void load(lexer_t &lexer, const token_t &token);

	//context_t save(string space, string name);
};

}
