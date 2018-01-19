#pragma once

#include <parse/token.h>
#include <parse/lexer.h>
#include <parse/message.h>
#include <parse/grammar.h>

namespace parse
{

struct branch_t
{
	branch_t();
	branch_t(const grammar_t::rule &rule, int parent, intptr_t offset, bool keep);
	branch_t(grammar_t::const_iterator point, int parent, intptr_t offset);
	~branch_t();

	token_t tree;
	grammar_t::const_links branches;
	grammar_t::const_iterator curr;
	int parent;
	int branch;
	bool keep;
};

struct branches_t
{
	branches_t();
	branches_t(const grammar_t::rule &rule, intptr_t offset);
	~branches_t();

	std::vector<branch_t> elems;
	int frame;

	operator bool();
	void push(grammar_t::const_iterator point, intptr_t offset);
	void pop();
	
	void push_frame(const grammar_t::rule &rule, intptr_t offset, bool keep);
	void pop_frame(intptr_t offset);
	bool rewind(lexer_t &lexer, std::vector<message> *msgs = NULL);
	void advance();

	branch_t &back();
	grammar_t::const_iterator &curr();

	token_t collapse_frame(int &index);
	token_t collapse();
};

}
