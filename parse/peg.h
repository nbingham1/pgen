#pragma once

#include "grammar.h"

struct peg_t : grammar
{
	int load_instance();
	int load_string();
	int load_definition();
	int load_sequence();
	int load_grammar();
	int load_choice();
	int load_term();
};

