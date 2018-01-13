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

		class_t space;
		space.add(' ');
		space.add('\t');
		space.add('\n');
		space.add('\r');
		
		iterator n[7];
		n[0] = insert(new stem(load_instance()));
		n[1] = insert(new stem(load_space()));
		n[2] = insert(new keyword("="));
		n[3] = insert(new stem(load_space()));
		n[4] = insert(new stem(load_choice()));
		n[5] = insert(new stem(load_space()));
		n[6] = insert(new keyword(";"));
		
		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[2].link(n[3]);
		n[3].link(n[4]);
		n[4].link(n[5]);
		n[5].link(n[6]);
		n[6].link(end());
	}

	return result;
}

int peg_t::load_sequence()
{
	static int result = (int)rules.size();
	
	if (result >= (int)rules.size())
	{
		rules.push_back(rule("sequence"));

		iterator n[2];
		n[0] = insert(new stem(load_term()));
		n[1] = insert(new stem(load_space()));
		
		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[0].link(end());
		n[1].link(n[0]);
	}

	return result;
}

int peg_t::load_grammar()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{
		rules.push_back(rule("grammar"));

		class_t *eof = new class_t();
		eof->add('\0');

		iterator n[2];
		n[0] = insert(new stem(load_definition()));
		n[1] = insert(new stem(load_space()));
		n[2] = insert(eof);

		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[1].link(n[0]);
		n[1].link(n[2]);
		n[2].link(end());
	}

	return result;
}

int peg_t::load_choice()
{
	static int result = (int)rules.size();
	
	if (result >= (int)rules.size())
	{
		rules.push_back(rule("choice"));

		iterator n[5];
		n[0] = insert(new stem(load_sequence()));
		n[1] = insert(new stem(load_space()));
		n[2] = insert(new keyword("|"));
		n[3] = insert(new stem(load_space()));
		n[4] = insert(new stem(load_sequence()));		

		rules[result].push(n[0]);
		n[0].link(n[1]);
		n[0].link(end());
		n[1].link(n[2]);
		n[2].link(n[3]);
		n[3].link(n[4]);
		n[4].link(n[1]);
		n[4].link(end());
	}

	return result;
}

int peg_t::load_term()
{
	static int result = (int)rules.size();
	
	if (result >= (int)rules.size())
	{
		rules.push_back(rule("term"));

		iterator n[10];
		n[0] = insert(new keyword("("));
		n[1] = insert(new stem(load_space()));
		n[2] = insert(new stem(load_choice()));
		n[3] = insert(new stem(load_space()));
		n[4] = insert(new keyword(")"));
		n[5] = insert(new stem(load_string()));
		n[6] = insert(new stem(load_instance()));
		n[7] = insert(new keyword("\?"));
		n[8] = insert(new keyword("*"));
		n[9] = insert(new keyword("+"));

		rules[result].push(n[0]);
		rules[result].push(n[5]);
		rules[result].push(n[6]);
		n[0].link(n[1]);
		n[1].link(n[2]);
		n[2].link(n[3]);
		n[3].link(n[4]);
		n[4].link(n[7]);
		n[4].link(n[8]);
		n[4].link(n[9]);
		n[4].link(end());
		n[5].link(n[7]);
		n[5].link(n[8]);
		n[5].link(n[9]);
		n[5].link(end());
		n[6].link(n[7]);
		n[6].link(n[8]);
		n[6].link(n[9]);
		n[6].link(end());
		n[7].link(end());
		n[8].link(end());
		n[9].link(end());
	}

	return result;
}

int peg_t::load_space()
{
	static int result = (int)rules.size();

	if (result >= (int)rules.size())
	{
		rules.push_back(rule("-", false));

		class_t *space = new class_t();
		space->add(' ');
		space->add('\t');
		space->add('\n');
		space->add('\r');
		
		iterator n[4];
		n[0] = insert(space);
		
		rules[result].push(n[0]);
		rules[result].push(end());
		n[0].link(n[0]);
		n[0].link(end());
	}

	return result;
}

