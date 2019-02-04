#pragma once

#include <parse/grammar.h>

namespace parse
{

struct peg_t
{
	int32_t PEG;
	int32_t DEFINITION;
	int32_t IMPORT;
	int32_t CHOICE;
	int32_t SEQUENCE;
	int32_t TERM;
	int32_t NAME;

	void load(grammar_t &grammar);
};

}

