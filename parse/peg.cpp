#include "peg.h"

int peg_t::load_instance()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{
		rules.push_back(rule("instance"));

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
		
		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[0].link(end());
		n[1].link(n[1]);
		n[1].link(end());
	}
	
	return result;
}

int peg_t::load_string()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{
		rules.push_back(rule("string"));
	
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
		
		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[0].link(n[2]);
		n[1].link(n[1]);
		n[1].link(n[2]);
		n[2].link(end());
	}
	
	return result;
}

int peg_t::load_definition()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{
		rules.push_back(rule("definition"));
	
		iterator n[4];
		n[0] = insert(new stem(load_instance()));
		n[1] = insert(new keyword("="));
		n[2] = insert(new stem(load_choice()));
		n[3] = insert(new keyword(";"));
		
		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[2].link(n[3]);
		n[3].link(end());
	}

	return result;
}

int peg_t::load_sequence()
{
	static int result = (int)rules.size();
	
	if (result >= (int)rules.size())
	{
		rules.push_back(rule("sequence"));

		iterator n[1];
		n[0] = insert(new stem(load_term()));
		
		rules[result].push(n[0]);
		n[0].link(n[0]);
		n[0].link(end());
	}

	return result;
}

int peg_t::load_grammar()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{
		rules.push_back(rule("grammar"));

		iterator n[2];
		n[0] = insert(new stem(load_definition()));
		n[1] = insert(new eof());

		rules[result].push(n[0]);
		n[0].link(n[0]);
		n[0].link(n[1]);
		n[1].link(end());
	}

	return result;
}

int peg_t::load_choice()
{
	static int result = (int)rules.size();
	
	if (result >= (int)rules.size())
	{
		rules.push_back(rule("choice"));

		iterator n[3];
		n[0] = insert(new stem(load_sequence()));
		n[1] = insert(new keyword("|"));
		n[2] = insert(new stem(load_sequence()));		

		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[0].link(end());
		n[1].link(n[2]);
		n[2].link(n[1]);
		n[2].link(end());
	}

	return result;
}

int peg_t::load_term()
{
	static int result = (int)rules.size();
	
	if (result >= (int)rules.size())
	{
		rules.push_back(rule("term"));

		iterator n[8];
		n[0] = insert(new keyword("("));
		n[1] = insert(new stem(load_choice()));
		n[2] = insert(new keyword(")"));
		n[3] = insert(new stem(load_string()));
		n[4] = insert(new stem(load_instance()));
		n[5] = insert(new keyword("\?"));
		n[6] = insert(new keyword("*"));
		n[7] = insert(new keyword("+"));

		rules[result].push(n[0]);
		rules[result].push(n[3]);
		rules[result].push(n[4]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[2].link(n[5]);
		n[2].link(n[6]);
		n[2].link(n[7]);
		n[2].link(end());
		n[3].link(n[5]);
		n[3].link(n[6]);
		n[3].link(n[7]);
		n[3].link(end());
		n[4].link(n[5]);
		n[4].link(n[6]);
		n[4].link(n[7]);
		n[4].link(end());
		n[5].link(end());
		n[6].link(end());
		n[7].link(end());
	}

	return result;
}

