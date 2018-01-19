/*
 * message.h
 *
 *  Created on: May 26, 2017
 *      Author: nbingham
 */

#pragma once

#include <parse/lexer.h>

namespace parse
{

struct message
{
	message();
	message(int type, std::string txt);
	message(int type, std::string txt, lexer_t &lexer, bool has_ctx = false, intptr_t begin = -1, intptr_t end = -1);
	~message();

	enum {
		fail = 0,
		error = 1,
		warning = 2,
		note = 3,
	};

	std::string file;
	intptr_t offset;
	intptr_t length;
	intptr_t column;
	intptr_t line;
	
	int type;
	std::string txt;
	std::string ctx;

	std::string typestr();
	void emit();
};

}
