#include "message.h"

message::message()
{
	line = -1;
	len = -1;
}

message::message(int type, string txt)
{
	this->type = type;
	this->len = 0;
	this->txt = txt;
}

message::message(int type, string txt, const lex &lexer, int len)
{
	this->type = type;
	this->txt = txt;

	this->file = lexer.name;
	this->offset = lexer.offset;
	this->column = lexer.column;
	this->line = lexer.line;
	this->len = len;
}

message::~message()
{
}

string message::typestr()
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
#ifdef DEBUG
	printf("%s:%d ", file.c_str(), line);
#endif

	if (loc)
		printf("%s ", loc.report().c_str());
	printf("%s: %s\n", typestr().c_str(), txt.c_str());
	if (loc) {
		printf("%s", loc.line->c_str());
		printf("%s", loc.pointer(len).c_str());
	}
}
