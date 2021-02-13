#pragma once

#include <string>
#include <vector>

#include <pgen/lexer.h>

namespace pgen
{

struct token_t
{
	token_t(intptr_t begin = -1, intptr_t end = -1);
	~token_t();

	int32_t type;
	intptr_t begin;
	intptr_t end;
	std::vector<token_t> tokens;

	operator bool();
	void append(const token_t &t);
	void extend(const token_t &t);
	void skip(const token_t &t);
	void clear();
	
	void emit(lexer_t &lexer, std::string tab = "");
};

}
