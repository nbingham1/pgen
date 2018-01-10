#include "peg.h"

int peg_t::load_instance()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{	
		class_t *first = new class_t();
		first->add('a', 'z');
		first->add('A', 'Z');
		first->add('_');
	
		class_t *second = new class_t();
		second->add('a', 'z');
		second->add('A', 'Z');
		second->add('0', '9');
		second->add('_');

		iterator n[2];
		n[0] = insert(first);
		n[1] = insert(second);
		n[0].link(n[1]);
		n[0].link(end());
		n[1].link(n[1]);
		n[1].link(end());

		rule instance;
		instance.start.push_back(n[0]);
		instance.name = "instance";
		instance.keep = true;
		rules.push_back(instance);
	}
	
	return result;
}

int peg_t::load_string()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{	
		class_t *first = new class_t();
		first->add('\"');
	
		class_t *second = new class_t();
		second->invert = true;
		second->add('\"');

		class_t *third = new class_t();
		third->add('\"');

		iterator n[3];
		n[0] = insert(first);
		n[1] = insert(second);
		n[2] = insert(third);
		n[0].link(n[1]);
		n[0].link(n[2]);
		n[1].link(n[1]);
		n[1].link(n[2]);
		n[2].link(end());

		rule str;
		str.start.push_back(n[0]);
		str.name = "string";
		str.keep = true;
		rules.push_back(str);
	}
	
	return result;
}


/*
peg_t::iterator peg_t::load_definition()
{
	iterator result = definitions.insert("definition", NULL);
	if (result->value == NULL)
	{
		syntax_t *sym = new syntax_t("definition");
		result->value = sym;

		syntax_t::graph::iterator n[4];
		n[0] = sym->syntax.insert(load_instance());
		n[1] = sym->syntax.insert(load_keyword(string("=", 1)));
		n[2] = sym->syntax.insert(load_choice());
		n[3] = sym->syntax.insert(load_keyword(string(";", 1)));
		sym->start.push_back(n[0]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[2].link(n[3]);
		n[3].link(sym->syntax.end());
	}
	return result;
}

peg_t::iterator peg_t::load_sequence()
{
	iterator result = definitions.insert("sequence", NULL);
	if (result->value == NULL)
	{
		syntax_t *sym = new syntax_t("sequence");
		result->value = sym;

		syntax_t::graph::iterator n[1];
		n[0] = sym->syntax.insert(load_term());
		sym->start.push_back(n[0]);
		n[0].link(n[0]);
		n[0].link(sym->syntax.end());
	}
	return result;
}

peg_t::iterator peg_t::load_state()
{
	iterator result = definitions.insert("state", NULL);
	if (result->value == NULL)
	{
		syntax_t *sym = new syntax_t("state");
		result->value = sym;

		syntax_t::graph::iterator n[5];
		n[0] = sym->syntax.insert(load_instance());
		n[1] = sym->syntax.insert(load_keyword(string(":", 1)));
		n[2] = sym->syntax.insert(load_text());
		n[3] = sym->syntax.insert(load_instance());
		n[4] = sym->syntax.insert(load_keyword(string(";", 1)));
		sym->start.push_back(n[0]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[1].link(n[3]);
		n[1].link(n[4]);
		n[2].link(n[2]);
		n[2].link(n[3]);
		n[2].link(n[4]);
		n[3].link(n[2]);
		n[3].link(n[3]);
		n[3].link(n[4]);
		n[4].link(sym->syntax.end());
	}
	return result;
}

peg_t::iterator peg_t::load_grammar()
{
	iterator result = definitions.insert("grammar", NULL);
	if (result->value == NULL)
	{
		syntax_t *sym = new syntax_t("grammar");
		result->value = sym;

		syntax_t::graph::iterator n[3];
		n[0] = sym->syntax.insert(load_definition());
		n[1] = sym->syntax.insert(load_state());
		n[2] = sym->syntax.insert(load_keyword(string("end", 3)));
		sym->start.push_back(n[0]);
		sym->start.push_back(n[1]);
		sym->start.push_back(n[2]);
		n[0].link(n[0]);
		n[0].link(n[1]);
		n[0].link(n[2]);
		n[1].link(n[0]);
		n[1].link(n[1]);
		n[1].link(n[2]);
		n[2].link(sym->syntax.end());
	}
	return result;
}

peg_t::iterator peg_t::load_choice()
{
	iterator result = definitions.insert("choice", NULL);
	if (result->value == NULL)
	{
		syntax_t *sym = new syntax_t("choice");
		result->value = sym;

		syntax_t::graph::iterator n[3];
		n[0] = sym->syntax.insert(load_sequence());
		n[1] = sym->syntax.insert(load_keyword(string("/", 1)));
		n[2] = sym->syntax.insert(load_sequence());
		sym->start.push_back(n[0]);
		n[0].link(n[1]);
		n[0].link(sym->syntax.end());
		n[1].link(n[2]);
		n[2].link(n[1]);
		n[2].link(sym->syntax.end());
	}
	return result;
}

peg_t::iterator peg_t::load_term()
{
	iterator result = definitions.insert("term", NULL);
	if (result->value == NULL)
	{
		syntax_t *sym = new syntax_t("term");
		result->value = sym;

		syntax_t::graph::iterator n[8];
		n[0] = sym->syntax.insert(load_keyword(string("(", 1)));
		n[1] = sym->syntax.insert(load_choice());
		n[2] = sym->syntax.insert(load_keyword(string(")", 1)));
		n[3] = sym->syntax.insert(load_text());
		n[4] = sym->syntax.insert(load_instance());
		n[5] = sym->syntax.insert(load_keyword(string("\?", 1)));
		n[6] = sym->syntax.insert(load_keyword(string("*", 1)));
		n[7] = sym->syntax.insert(load_keyword(string("+", 1)));
		sym->start.push_back(n[0]);
		sym->start.push_back(n[3]);
		sym->start.push_back(n[4]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[2].link(n[5]);
		n[2].link(n[6]);
		n[2].link(n[7]);
		n[2].link(sym->syntax.end());
		n[3].link(n[5]);
		n[3].link(n[6]);
		n[3].link(n[7]);
		n[3].link(sym->syntax.end());
		n[4].link(n[5]);
		n[4].link(n[6]);
		n[4].link(n[7]);
		n[4].link(sym->syntax.end());
		n[5].link(sym->syntax.end());
		n[6].link(sym->syntax.end());
		n[7].link(sym->syntax.end());
	}
	return result;
}

}
*/

