/*
 * message.h
 *
 *  Created on: May 26, 2017
 *      Author: nbingham
 */

#pragma once

#include "lex.h"

struct message
{
	message();
	message(int type, string txt);
	message(int type, string txt, lex &lexer, bool has_ctx = false, intptr_t begin = -1, intptr_t end = -1);
	~message();

	enum {
		fail = 0,
		error = 1,
		warning = 2,
		note = 3,
	};

	string file;
	intptr_t offset;
	intptr_t length;
	intptr_t column;
	intptr_t line;
	
	int type;
	string txt;
	string ctx;

	string typestr();
	void emit();
};

