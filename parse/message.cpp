#include <parse/message.h>

namespace parse
{

message::message()
{
	this->type = -1;
	
	offset = -1;
	length = 0;
	column = -1;
	line = -1;
}

message::message(int type, std::string txt)
{
	this->type = type;
	this->txt = txt;
	
	offset = -1;
	length = 0;
	column = -1;
	line = -1;
}

message::message(int type, std::string txt, lexer_t &lexer, bool has_ctx, intptr_t begin, intptr_t end)
{
	this->type = type;
	this->txt = txt;

	file = lexer.name;

	if (begin < 0)
		begin = lexer.offset;
	if (end < 0)
		end = begin;

	offset = begin;
	length = end - begin;
	line = lexer.lineof(begin);
	column = offset - lexer.lines[line];

	if (has_ctx)
	{
		ctx = lexer.getline(line);
		if (ctx.back() != '\n')
			ctx.push_back('\n');

		for (int i = 0; i < column; i++)
		{
			if (ctx[i] < 32 || ctx[i] == 127)
				ctx.push_back(ctx[i]);
			else
				ctx.push_back(' ');
		}
		ctx += '^';
		for (int i = 0; i < length-1; i++)
			ctx += '~';
		ctx += '\n';
	}
}

message::~message()
{
}

std::string message::typestr()
{
	switch (type)
	{
		case fail: return "fail";
		case error: return "error";
		case warning: return "warning";
		default: return "note";
	}
}

void message::emit()
{
	if (file.size() > 0)
		printf("%s:", file.c_str());
	else
		printf(":");

	if (line >= 0)
		printf("%ld:", line);
	else
		printf(":");

	if (column >= 0)
		printf("%ld ", column);
	else
		printf(" ");

	printf("%s: %s\n", typestr().c_str(), txt.c_str());
	if (ctx.size() > 0)
		printf("%s", ctx.c_str());
}

}
