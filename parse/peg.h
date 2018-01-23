#pragma once

#include <parse/grammar.h>

namespace parse
{

struct peg_t : parse::grammar_t
{
	peg_t();
	~peg_t();
};

}

