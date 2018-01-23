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

	iterator n[39];
	n[0] = insert(new stem(1, 1));
	n[1] = insert(new stem(2, 1));
	n[2] = insert(new whitespace());
	n[3] = insert(new character("\\0"));
	n[4] = insert(new instance());
	n[5] = insert(new whitespace());
	n[6] = insert(new keyword("="));
	n[7] = insert(new whitespace());
	n[8] = insert(new stem(3, 1));
	n[9] = insert(new whitespace());
	n[10] = insert(new keyword(";"));
	n[11] = insert(new keyword("import"));
	n[12] = insert(new whitespace());
	n[13] = insert(new text());
	n[14] = insert(new whitespace());
	n[15] = insert(new keyword(";"));
	n[16] = insert(new stem(4, 1));
	n[17] = insert(new whitespace());
	n[18] = insert(new keyword("|"));
	n[19] = insert(new whitespace());
	n[20] = insert(new stem(4, 1));
	n[21] = insert(new stem(5, 1));
	n[22] = insert(new whitespace());
	n[23] = insert(new stem(5, 1));
	n[24] = insert(new keyword("("));
	n[25] = insert(new whitespace());
	n[26] = insert(new stem(3, 1));
	n[27] = insert(new whitespace());
	n[28] = insert(new keyword(")"));
	n[29] = insert(new text());
	n[30] = insert(new stem(6, 1));
	n[31] = insert(new character_class());
	n[32] = insert(new whitespace());
	n[33] = insert(new keyword("?"));
	n[34] = insert(new keyword("*"));
	n[35] = insert(new keyword("+"));
	n[36] = insert(new instance());
	n[37] = insert(new keyword("::"));
	n[38] = insert(new instance());

	n[0].link(n[2]);
	n[1].link(n[2]);
	n[2].link(n[0]);
	n[2].link(n[1]);
	n[2].link(n[3]);
	n[3].link(end());
	n[4].link(n[5]);
	n[5].link(n[6]);
	n[6].link(n[7]);
	n[7].link(n[8]);
	n[8].link(n[9]);
	n[9].link(n[10]);
	n[10].link(end());
	n[11].link(n[12]);
	n[12].link(n[13]);
	n[13].link(n[14]);
	n[14].link(n[15]);
	n[15].link(end());
	n[16].link(n[17]);
	n[16].link(end());
	n[17].link(n[18]);
	n[18].link(n[19]);
	n[19].link(n[20]);
	n[20].link(n[17]);
	n[20].link(end());
	n[21].link(n[22]);
	n[21].link(end());
	n[22].link(n[23]);
	n[23].link(n[22]);
	n[23].link(end());
	n[24].link(n[25]);
	n[25].link(n[26]);
	n[26].link(n[27]);
	n[27].link(n[28]);
	n[28].link(n[32]);
	n[28].link(end());
	n[29].link(n[32]);
	n[29].link(end());
	n[30].link(n[32]);
	n[30].link(end());
	n[31].link(n[32]);
	n[31].link(end());
	n[32].link(n[33]);
	n[32].link(n[34]);
	n[32].link(n[35]);
	n[33].link(end());
	n[34].link(end());
	n[35].link(end());
	n[36].link(n[37]);
	n[36].link(end());
	n[37].link(n[38]);
	n[38].link(end());

	rules[0].start.push_back(n[0]);
	rules[0].start.push_back(n[1]);
	rules[0].start.push_back(n[3]);
	rules[1].start.push_back(n[4]);
	rules[2].start.push_back(n[11]);
	rules[3].start.push_back(n[16]);
	rules[4].start.push_back(n[21]);
	rules[5].start.push_back(n[24]);
	rules[5].start.push_back(n[29]);
	rules[5].start.push_back(n[30]);
	rules[5].start.push_back(n[31]);
	rules[6].start.push_back(n[36]);
}

peg_t::~peg_t()
{
}

}

