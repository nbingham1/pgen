#pragma once

#include <parse/grammar.h>

namespace parse
{

struct peg_t : grammar_t
{
	peg_t();
	~peg_t();

	int load_peg();
	int load_definition();
	int load_choice();
	int load_sequence();
	int load_term();
};

}
