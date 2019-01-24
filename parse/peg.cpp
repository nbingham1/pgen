#include "peg.h"

#include <parse/default.h>

namespace parse
{

peg_t::peg_t()
{
	rules.push_back(rule("peg::peg", true));
	rules.push_back(rule("peg::definition", true));
	rules.push_back(rule("peg::import", true));
	rules.push_back(rule("peg::choice", false));
	rules.push_back(rule("peg::sequence", false));
	rules.push_back(rule("peg::term", false));
	rules.push_back(rule("peg::name", false));

	iterator n[42];
	n[0] = insert(new stem(1, true));
	n[1] = insert(new stem(2, true));
	n[2] = insert(new whitespace(true, false));
	n[3] = insert(new character("\\0", true));
	n[4] = insert(new instance(true));
	n[5] = insert(new whitespace(true, false));
	n[6] = insert(new keyword("@=", true));
	n[7] = insert(new keyword("=", true));
	n[8] = insert(new whitespace(true, false));
	n[9] = insert(new stem(3, true));
	n[10] = insert(new whitespace(true, false));
	n[11] = insert(new keyword(";", true));
	n[12] = insert(new keyword("import", true));
	n[13] = insert(new whitespace(true, false));
	n[14] = insert(new text(true));
	n[15] = insert(new whitespace(true, false));
	n[16] = insert(new keyword(";", true));
	n[17] = insert(new stem(4, true));
	n[18] = insert(new whitespace(true, false));
	n[19] = insert(new keyword("|", true));
	n[20] = insert(new whitespace(true, false));
	n[21] = insert(new stem(4, true));
	n[22] = insert(new stem(5, true));
	n[23] = insert(new whitespace(true, false));
	n[24] = insert(new stem(5, true));
	n[25] = insert(new keyword("(", true));
	n[26] = insert(new whitespace(true, false));
	n[27] = insert(new stem(3, true));
	n[28] = insert(new whitespace(true, false));
	n[29] = insert(new keyword(")", true));
	n[30] = insert(new keyword("~", true));
	n[31] = insert(new whitespace(true, false));
	n[32] = insert(new text(true));
	n[33] = insert(new stem(6, true));
	n[34] = insert(new character_class(true));
	n[35] = insert(new whitespace(true, false));
	n[36] = insert(new keyword("?", true));
	n[37] = insert(new keyword("*", true));
	n[38] = insert(new keyword("+", true));
	n[39] = insert(new instance(true));
	n[40] = insert(new keyword("::", true));
	n[41] = insert(new instance(true));

	n[0].link(n[2]);
	n[1].link(n[2]);
	n[2].link(n[0]);
	n[2].link(n[1]);
	n[2].link(n[3]);
	n[3].link(end());
	n[4].link(n[5]);
	n[5].link(n[6]);
	n[5].link(n[7]);
	n[6].link(n[8]);
	n[7].link(n[8]);
	n[8].link(n[9]);
	n[9].link(n[10]);
	n[10].link(n[11]);
	n[11].link(end());
	n[12].link(n[13]);
	n[13].link(n[14]);
	n[14].link(n[15]);
	n[15].link(n[16]);
	n[16].link(end());
	n[17].link(n[18]);
	n[17].link(end());
	n[18].link(n[19]);
	n[19].link(n[20]);
	n[20].link(n[21]);
	n[21].link(n[18]);
	n[21].link(end());
	n[22].link(n[23]);
	n[22].link(end());
	n[23].link(n[24]);
	n[24].link(n[23]);
	n[24].link(end());
	n[25].link(n[26]);
	n[26].link(n[27]);
	n[27].link(n[28]);
	n[28].link(n[29]);
	n[29].link(n[35]);
	n[29].link(end());
	n[30].link(n[31]);
	n[31].link(n[32]);
	n[31].link(n[33]);
	n[31].link(n[34]);
	n[32].link(n[35]);
	n[32].link(end());
	n[33].link(n[35]);
	n[33].link(end());
	n[34].link(n[35]);
	n[34].link(end());
	n[35].link(n[36]);
	n[35].link(n[37]);
	n[35].link(n[38]);
	n[36].link(end());
	n[37].link(end());
	n[38].link(end());
	n[39].link(n[40]);
	n[39].link(end());
	n[40].link(n[41]);
	n[41].link(end());

	rules[0].start.push_back(n[0]);
	rules[0].start.push_back(n[1]);
	rules[0].start.push_back(n[3]);
	rules[1].start.push_back(n[4]);
	rules[2].start.push_back(n[12]);
	rules[3].start.push_back(n[17]);
	rules[4].start.push_back(n[22]);
	rules[5].start.push_back(n[25]);
	rules[5].start.push_back(n[30]);
	rules[5].start.push_back(n[32]);
	rules[5].start.push_back(n[33]);
	rules[5].start.push_back(n[34]);
	rules[6].start.push_back(n[39]);
}

peg_t::~peg_t()
{
}

}

