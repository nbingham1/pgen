#pragma once

#include "grammar.h"

struct peg_t : grammar
{
	int load_instance();
	int load_string();
};

