#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include <parse/token.h>
#include <parse/message.h>
#include <parse/lexer.h>

namespace parse
{

struct parsing
{
	parsing(intptr_t begin = -1, intptr_t end = -1);
	~parsing();

	int stem;
	token_t tree;
	std::vector<message> msgs;

	void emit(lexer_t &lexer);
};

struct symbol_t 
{
	symbol_t(bool keep = true);
	virtual ~symbol_t();

	bool keep;
	
	std::vector<symbol_t*> next;
	symbol_t *right;

	virtual parsing parse(lexer_t &lexer) const = 0;
	virtual symbol_t *clone(int rule_offset = 0) const = 0;
	virtual std::string emit() const = 0;
};

struct rule_t
{
	rule_t();
	rule_t(int32_t type, std::string name, bool atomic = false);
	~rule_t();

	int32_t type;
	std::string name;
	std::vector<symbol_t*> start;
	bool atomic;
};

struct branch_t
{
	branch_t();
	branch_t(const rule_t &rule, int parent, intptr_t offset, bool keep);
	branch_t(symbol_t *point, int parent, intptr_t offset);
	~branch_t();

	std::string name;
	token_t tree;
	std::vector<symbol_t*> branches;
	symbol_t *curr;
	int parent;
	int branch;
	bool keep;
	bool atomic;
};

struct branches_t
{
	branches_t();
	branches_t(const rule_t &rule, intptr_t offset);
	~branches_t();

	std::vector<branch_t> elems;
	int frame;

	operator bool();
	void push(symbol_t *point, intptr_t offset);
	void pop();
	
	void push_frame(const rule_t &rule, intptr_t offset, bool keep);
	void pop_frame(intptr_t offset);
	bool rewind(lexer_t &lexer, std::vector<message> *msgs = NULL);
	void advance();

	branch_t &back();
	const symbol_t *curr();

	void collapse_frame(int index);
	token_t collect_frame(int &index);
	token_t collect();
};

struct grammar_t
{
	grammar_t();
	~grammar_t();

	symbol_t *symbols;
	std::vector<rule_t> rules;

	symbol_t *insert(symbol_t *sym);

	parsing parse(lexer_t &lexer, int index = 0);
};

}
