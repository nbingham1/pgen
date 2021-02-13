#include "peg.h"

#include <pgen/default.h>

namespace pgen
{

void peg_t::load(grammar_t &grammar)
{
	PEG = grammar.rules.size();
	grammar.rules.push_back(rule_t(PEG, "peg::peg", true, true));
	DEFINITION = grammar.rules.size();
	grammar.rules.push_back(rule_t(DEFINITION, "peg::definition", true, true));
	IMPORT = grammar.rules.size();
	grammar.rules.push_back(rule_t(IMPORT, "peg::import", true, true));
	CHOICE = grammar.rules.size();
	grammar.rules.push_back(rule_t(CHOICE, "peg::choice", true, true));
	SEQUENCE = grammar.rules.size();
	grammar.rules.push_back(rule_t(SEQUENCE, "peg::sequence", true, true));
	TERM = grammar.rules.size();
	grammar.rules.push_back(rule_t(TERM, "peg::term", true, true));
	NAME = grammar.rules.size();
	grammar.rules.push_back(rule_t(NAME, "peg::name", true, true));

	symbol_t *n[44];
	n[0] = grammar.insert(new instance(true));
	n[1] = grammar.insert(new keyword("::", true));
	n[2] = grammar.insert(new instance(true));
	n[3] = grammar.insert(new keyword("+", true));
	n[4] = grammar.insert(new keyword("*", true));
	n[5] = grammar.insert(new keyword("?", true));
	n[6] = grammar.insert(new whitespace(true, false));
	n[7] = grammar.insert(new character_class(true));
	n[8] = grammar.insert(new stem(6, true));
	n[9] = grammar.insert(new text(true));
	n[10] = grammar.insert(new whitespace(true, false));
	n[11] = grammar.insert(new keyword("~", true));
	n[12] = grammar.insert(new keyword(")", true));
	n[13] = grammar.insert(new whitespace(true, false));
	n[14] = grammar.insert(new stem(3, true));
	n[15] = grammar.insert(new whitespace(true, false));
	n[16] = grammar.insert(new keyword("(", true));
	n[17] = grammar.insert(new stem(5, true));
	n[18] = grammar.insert(new whitespace(true, false));
	n[19] = grammar.insert(new stem(5, true));
	n[20] = grammar.insert(new stem(4, true));
	n[21] = grammar.insert(new whitespace(true, false));
	n[22] = grammar.insert(new keyword("|", true));
	n[23] = grammar.insert(new whitespace(true, false));
	n[24] = grammar.insert(new stem(4, true));
	n[25] = grammar.insert(new keyword(";", true));
	n[26] = grammar.insert(new whitespace(true, false));
	n[27] = grammar.insert(new text(true));
	n[28] = grammar.insert(new whitespace(true, false));
	n[29] = grammar.insert(new keyword("import", true));
	n[30] = grammar.insert(new keyword(";", false));
	n[31] = grammar.insert(new whitespace(true, false));
	n[32] = grammar.insert(new stem(3, true));
	n[33] = grammar.insert(new whitespace(true, false));
	n[34] = grammar.insert(new keyword("=", false));
	n[35] = grammar.insert(new keyword("@", true));
	n[36] = grammar.insert(new keyword("~", true));
	n[37] = grammar.insert(new whitespace(true, false));
	n[38] = grammar.insert(new instance(true));
	n[39] = grammar.insert(new character("\\0", true));
	n[40] = grammar.insert(new whitespace(true, false));
	n[41] = grammar.insert(new stem(2, true));
	n[42] = grammar.insert(new stem(1, true));
	n[43] = grammar.insert(new whitespace(true, false));

	n[0]->next.push_back(NULL);
	n[1]->next.push_back(n[0]);
	n[2]->next.push_back(n[1]);
	n[2]->next.push_back(NULL);
	n[3]->next.push_back(NULL);
	n[4]->next.push_back(NULL);
	n[5]->next.push_back(NULL);
	n[6]->next.push_back(n[5]);
	n[6]->next.push_back(n[4]);
	n[6]->next.push_back(n[3]);
	n[7]->next.push_back(n[6]);
	n[7]->next.push_back(NULL);
	n[8]->next.push_back(n[6]);
	n[8]->next.push_back(NULL);
	n[9]->next.push_back(n[6]);
	n[9]->next.push_back(NULL);
	n[10]->next.push_back(n[9]);
	n[10]->next.push_back(n[8]);
	n[10]->next.push_back(n[7]);
	n[11]->next.push_back(n[10]);
	n[12]->next.push_back(n[6]);
	n[12]->next.push_back(NULL);
	n[13]->next.push_back(n[12]);
	n[14]->next.push_back(n[13]);
	n[15]->next.push_back(n[14]);
	n[16]->next.push_back(n[15]);
	n[17]->next.push_back(n[18]);
	n[17]->next.push_back(NULL);
	n[18]->next.push_back(n[17]);
	n[19]->next.push_back(n[18]);
	n[19]->next.push_back(NULL);
	n[20]->next.push_back(n[23]);
	n[20]->next.push_back(NULL);
	n[21]->next.push_back(n[20]);
	n[22]->next.push_back(n[21]);
	n[23]->next.push_back(n[22]);
	n[24]->next.push_back(n[23]);
	n[24]->next.push_back(NULL);
	n[25]->next.push_back(NULL);
	n[26]->next.push_back(n[25]);
	n[27]->next.push_back(n[26]);
	n[28]->next.push_back(n[27]);
	n[29]->next.push_back(n[28]);
	n[30]->next.push_back(NULL);
	n[31]->next.push_back(n[30]);
	n[32]->next.push_back(n[31]);
	n[33]->next.push_back(n[32]);
	n[34]->next.push_back(n[33]);
	n[35]->next.push_back(n[34]);
	n[36]->next.push_back(n[35]);
	n[36]->next.push_back(n[34]);
	n[37]->next.push_back(n[36]);
	n[37]->next.push_back(n[35]);
	n[37]->next.push_back(n[34]);
	n[38]->next.push_back(n[37]);
	n[39]->next.push_back(NULL);
	n[40]->next.push_back(n[42]);
	n[40]->next.push_back(n[41]);
	n[40]->next.push_back(n[39]);
	n[41]->next.push_back(n[40]);
	n[42]->next.push_back(n[40]);
	n[43]->next.push_back(n[42]);
	n[43]->next.push_back(n[41]);
	n[43]->next.push_back(n[39]);

	grammar.rules[0].start.push_back(n[43]);
	grammar.rules[1].start.push_back(n[38]);
	grammar.rules[2].start.push_back(n[29]);
	grammar.rules[3].start.push_back(n[24]);
	grammar.rules[4].start.push_back(n[19]);
	grammar.rules[5].start.push_back(n[16]);
	grammar.rules[5].start.push_back(n[11]);
	grammar.rules[5].start.push_back(n[9]);
	grammar.rules[5].start.push_back(n[8]);
	grammar.rules[5].start.push_back(n[7]);
	grammar.rules[6].start.push_back(n[2]);
}

}

