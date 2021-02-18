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
	INSTANCE = grammar.rules.size();
	grammar.rules.push_back(rule_t(INSTANCE, "peg::instance", true, true));
	CHOICE = grammar.rules.size();
	grammar.rules.push_back(rule_t(CHOICE, "peg::choice", true, true));
	TEXT = grammar.rules.size();
	grammar.rules.push_back(rule_t(TEXT, "peg::text", true, true));
	SEQUENCE = grammar.rules.size();
	grammar.rules.push_back(rule_t(SEQUENCE, "peg::sequence", true, true));
	TERM = grammar.rules.size();
	grammar.rules.push_back(rule_t(TERM, "peg::term", true, true));
	NAME = grammar.rules.size();
	grammar.rules.push_back(rule_t(NAME, "peg::name", true, true));

	symbol_t *n[45];
	n[0] = grammar.insert(new regular_expression("[_a-zA-Z][_a-zA-Z0-9]*", true));
	n[1] = grammar.insert(new regular_expression("\\\"([^\\\"\\\\]|\\\\.)*\\\"", true));
	n[2] = grammar.insert(new stem(3, true));
	n[3] = grammar.insert(new regular_expression("::", true));
	n[4] = grammar.insert(new stem(3, true));
	n[5] = grammar.insert(new regular_expression("\\+", true));
	n[6] = grammar.insert(new regular_expression("\\*", true));
	n[7] = grammar.insert(new regular_expression("\\?", true));
	n[8] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[9] = grammar.insert(new stem(8, true));
	n[10] = grammar.insert(new stem(5, true));
	n[11] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[12] = grammar.insert(new regular_expression("~", true));
	n[13] = grammar.insert(new regular_expression("\\)", true));
	n[14] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[15] = grammar.insert(new stem(4, true));
	n[16] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[17] = grammar.insert(new regular_expression("\\(", true));
	n[18] = grammar.insert(new stem(7, true));
	n[19] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[20] = grammar.insert(new stem(7, true));
	n[21] = grammar.insert(new stem(6, true));
	n[22] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[23] = grammar.insert(new regular_expression("\\|", true));
	n[24] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[25] = grammar.insert(new stem(6, true));
	n[26] = grammar.insert(new regular_expression(";", true));
	n[27] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[28] = grammar.insert(new stem(5, true));
	n[29] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[30] = grammar.insert(new regular_expression("import", true));
	n[31] = grammar.insert(new regular_expression(";", false));
	n[32] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[33] = grammar.insert(new stem(4, true));
	n[34] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[35] = grammar.insert(new regular_expression("=", false));
	n[36] = grammar.insert(new regular_expression("@", true));
	n[37] = grammar.insert(new regular_expression("~", true));
	n[38] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[39] = grammar.insert(new stem(3, true));
	n[40] = grammar.insert(new regular_expression("\\0", true));
	n[41] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));
	n[42] = grammar.insert(new stem(2, true));
	n[43] = grammar.insert(new stem(1, true));
	n[44] = grammar.insert(new regular_expression("[ \\t\\n\\r]*", false));

	n[0]->next.push_back(NULL);
	n[1]->next.push_back(NULL);
	n[2]->next.push_back(NULL);
	n[3]->next.push_back(n[2]);
	n[4]->next.push_back(n[3]);
	n[4]->next.push_back(NULL);
	n[5]->next.push_back(NULL);
	n[6]->next.push_back(NULL);
	n[7]->next.push_back(NULL);
	n[8]->next.push_back(n[7]);
	n[8]->next.push_back(n[6]);
	n[8]->next.push_back(n[5]);
	n[9]->next.push_back(n[8]);
	n[9]->next.push_back(NULL);
	n[10]->next.push_back(n[8]);
	n[10]->next.push_back(NULL);
	n[11]->next.push_back(n[10]);
	n[11]->next.push_back(n[9]);
	n[12]->next.push_back(n[11]);
	n[13]->next.push_back(n[8]);
	n[13]->next.push_back(NULL);
	n[14]->next.push_back(n[13]);
	n[15]->next.push_back(n[14]);
	n[16]->next.push_back(n[15]);
	n[17]->next.push_back(n[16]);
	n[18]->next.push_back(n[19]);
	n[18]->next.push_back(NULL);
	n[19]->next.push_back(n[18]);
	n[20]->next.push_back(n[19]);
	n[20]->next.push_back(NULL);
	n[21]->next.push_back(n[24]);
	n[21]->next.push_back(NULL);
	n[22]->next.push_back(n[21]);
	n[23]->next.push_back(n[22]);
	n[24]->next.push_back(n[23]);
	n[25]->next.push_back(n[24]);
	n[25]->next.push_back(NULL);
	n[26]->next.push_back(NULL);
	n[27]->next.push_back(n[26]);
	n[28]->next.push_back(n[27]);
	n[29]->next.push_back(n[28]);
	n[30]->next.push_back(n[29]);
	n[31]->next.push_back(NULL);
	n[32]->next.push_back(n[31]);
	n[33]->next.push_back(n[32]);
	n[34]->next.push_back(n[33]);
	n[35]->next.push_back(n[34]);
	n[36]->next.push_back(n[35]);
	n[37]->next.push_back(n[36]);
	n[37]->next.push_back(n[35]);
	n[38]->next.push_back(n[37]);
	n[38]->next.push_back(n[36]);
	n[38]->next.push_back(n[35]);
	n[39]->next.push_back(n[38]);
	n[40]->next.push_back(NULL);
	n[41]->next.push_back(n[43]);
	n[41]->next.push_back(n[42]);
	n[41]->next.push_back(n[40]);
	n[42]->next.push_back(n[41]);
	n[43]->next.push_back(n[41]);
	n[44]->next.push_back(n[43]);
	n[44]->next.push_back(n[42]);
	n[44]->next.push_back(n[40]);

	grammar.rules[0].start.push_back(n[44]);
	grammar.rules[1].start.push_back(n[39]);
	grammar.rules[2].start.push_back(n[30]);
	grammar.rules[3].start.push_back(n[0]);
	grammar.rules[4].start.push_back(n[25]);
	grammar.rules[5].start.push_back(n[1]);
	grammar.rules[6].start.push_back(n[20]);
	grammar.rules[7].start.push_back(n[17]);
	grammar.rules[7].start.push_back(n[12]);
	grammar.rules[7].start.push_back(n[10]);
	grammar.rules[7].start.push_back(n[9]);
	grammar.rules[8].start.push_back(n[4]);
}

}

