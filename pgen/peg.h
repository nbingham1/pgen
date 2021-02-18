#pragma once

#include <pgen/grammar.h>

namespace pgen
{

struct peg_t
{
	int32_t PEG;
	int32_t DEFINITION;
	int32_t IMPORT;
	int32_t INSTANCE;
	int32_t CHOICE;
	int32_t TEXT;
	int32_t SEQUENCE;
	int32_t TERM;
	int32_t NAME;

	void load(grammar_t &grammar);
};

}

