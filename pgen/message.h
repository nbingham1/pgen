/*
 * message.h
 *
 *  Created on: May 26, 2017
 *      Author: nbingham
 */

#pragma once

#include <pgen/lexer.h>
#include <pgen/token.h>
#include <sstream>
#include <iostream>

namespace pgen
{

struct message
{
	enum {
		fail = 0,
		error = 1,
		warning = 2,
		note = 3,
	};

	int type;
	std::string file;
	long line;
	long column;
	long length;
	std::string text;
	std::string context;
};

std::ostream &operator<<(std::ostream &os, const message &msg);

struct messenger
{
	messenger(int type);
	~messenger();

	int type;
	std::string file;
	long line;
	long column;
	long length;
	std::stringstream text;
	std::string context;

	messenger &operator() (lexer_t &lexer, token_t token);
	messenger &operator() (lexer_t &lexer, intptr_t begin = -1, intptr_t end = -1);
	messenger &operator() (std::string file = "", long line = -1, long column = -1, long length = -1);

	operator message();
};

template <typename T>
messenger &operator<<(messenger &msg, T value)
{
	msg.text << value;
	return msg;
}

std::ostream &operator<<(std::ostream &os, const messenger &msg);

extern messenger fail;
extern messenger error;
extern messenger warning;
extern messenger note;

}
