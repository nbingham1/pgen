#include "peg.h"

#include <parse/default.h>

namespace parse
{

peg_t::peg_t()
{
	rules.push_back(rule("peg::peg", true));
	rules.push_back(rule("peg::definition", true));
	rules.push_back(rule("peg::import", true));
	rules.push_back(rule("peg::choice", true));
	rules.push_back(rule("peg::sequence", true));
	rules.push_back(rule("peg::term", true));
	rules.push_back(rule("peg::name", true));

	iterator n[43];
	n[0] = insert(new whitespace(true, false));
	n[1] = insert(new stem(1, true));
	n[2] = insert(new stem(2, true));
	n[3] = insert(new whitespace(true, false));
	n[4] = insert(new character("\\0", true));
	n[5] = insert(new instance(true));
	n[6] = insert(new whitespace(true, false));
	n[7] = insert(new keyword("@=", true));
	n[8] = insert(new keyword("=", true));
	n[9] = insert(new whitespace(true, false));
	n[10] = insert(new stem(3, true));
	n[11] = insert(new whitespace(true, false));
	n[12] = insert(new keyword(";", true));
	n[13] = insert(new keyword("import", true));
	n[14] = insert(new whitespace(true, false));
	n[15] = insert(new text(true));
	n[16] = insert(new whitespace(true, false));
	n[17] = insert(new keyword(";", true));
	n[18] = insert(new stem(4, true));
	n[19] = insert(new whitespace(true, false));
	n[20] = insert(new keyword("|", true));
	n[21] = insert(new whitespace(true, false));
	n[22] = insert(new stem(4, true));
	n[23] = insert(new stem(5, true));
	n[24] = insert(new whitespace(true, false));
	n[25] = insert(new stem(5, true));
	n[26] = insert(new keyword("(", true));
	n[27] = insert(new whitespace(true, false));
	n[28] = insert(new stem(3, true));
	n[29] = insert(new whitespace(true, false));
	n[30] = insert(new keyword(")", true));
	n[31] = insert(new keyword("~", true));
	n[32] = insert(new whitespace(true, false));
	n[33] = insert(new text(true));
	n[34] = insert(new stem(6, true));
	n[35] = insert(new character_class(true));
	n[36] = insert(new whitespace(true, false));
	n[37] = insert(new keyword("?", true));
	n[38] = insert(new keyword("*", true));
	n[39] = insert(new keyword("+", true));
	n[40] = insert(new instance(true));
	n[41] = insert(new keyword("::", true));
	n[42] = insert(new instance(true));

	n[0].link(n[1]);
	n[0].link(n[2]);
	n[0].link(n[4]);
	n[1].link(n[3]);
	n[2].link(n[3]);
	n[3].link(n[1]);
	n[3].link(n[2]);
	n[3].link(n[4]);
	n[4].link(end());
	n[5].link(n[6]);
	n[6].link(n[7]);
	n[6].link(n[8]);
	n[7].link(n[9]);
	n[8].link(n[9]);
	n[9].link(n[10]);
	n[10].link(n[11]);
	n[11].link(n[12]);
	n[12].link(end());
	n[13].link(n[14]);
	n[14].link(n[15]);
	n[15].link(n[16]);
	n[16].link(n[17]);
	n[17].link(end());
	n[18].link(n[19]);
	n[18].link(end());
	n[19].link(n[20]);
	n[20].link(n[21]);
	n[21].link(n[22]);
	n[22].link(n[19]);
	n[22].link(end());
	n[23].link(n[24]);
	n[23].link(end());
	n[24].link(n[25]);
	n[25].link(n[24]);
	n[25].link(end());
	n[26].link(n[27]);
	n[27].link(n[28]);
	n[28].link(n[29]);
	n[29].link(n[30]);
	n[30].link(n[36]);
	n[30].link(end());
	n[31].link(n[32]);
	n[32].link(n[33]);
	n[32].link(n[34]);
	n[32].link(n[35]);
	n[33].link(n[36]);
	n[33].link(end());
	n[34].link(n[36]);
	n[34].link(end());
	n[35].link(n[36]);
	n[35].link(end());
	n[36].link(n[37]);
	n[36].link(n[38]);
	n[36].link(n[39]);
	n[37].link(end());
	n[38].link(end());
	n[39].link(end());
	n[40].link(n[41]);
	n[40].link(end());
	n[41].link(n[42]);
	n[42].link(end());

	rules[0].start.push_back(n[0]);
	rules[1].start.push_back(n[5]);
	rules[2].start.push_back(n[13]);
	rules[3].start.push_back(n[18]);
	rules[4].start.push_back(n[23]);
	rules[5].start.push_back(n[26]);
	rules[5].start.push_back(n[31]);
	rules[5].start.push_back(n[33]);
	rules[5].start.push_back(n[34]);
	rules[5].start.push_back(n[35]);
	rules[6].start.push_back(n[40]);
}

peg_t::~peg_t()
{
}

}

