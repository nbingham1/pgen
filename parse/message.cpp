#include <parse/message.h>

namespace parse
{

messenger fail(message::fail);
messenger error(message::error);
messenger warning(message::warning);
messenger note(message::note);

std::ostream &operator<<(std::ostream &os, const message &msg)
{
	if (msg.file.size() > 0)
		os << msg.file << ":";

	if (msg.line >= 0)
		os << msg.line << ":";

	if (msg.column >= 0)
		os << msg.column << " ";

	switch (msg.type)
	{
		case message::fail: os << "fail: "; break;
		case message::error: os << "error: "; break;
		case message::warning: os << "warning: "; break;
		default: os << "note: "; break;
	}

	os << msg.text << std::endl;
	if (msg.context.size() > 0)
		os << msg.context;
	return os;
}

messenger::messenger(int type)
{
	this->type = type;
	context = false;
}

messenger::~messenger()
{
}

messenger &messenger::operator() (lexer_t &lexer, token_t token)
{
	return this->operator()(lexer, token.begin, token.end);
}

messenger &messenger::operator() (lexer_t &lexer, intptr_t begin, intptr_t end)
{
	file = lexer.name;
	line = lexer.lineof(begin);
	column = begin - lexer.lines[line];
	length = end - begin;
	
	text.clear();

	context = lexer.getline(line);
	if (context.back() != '\n')
		context.push_back('\n');
	int maxlen = (int)context.size()-1;

	for (int i = 0; i < column; i++)
	{
		if (context[i] < 32 || context[i] == 127)
			context.push_back(context[i]);
		else
			context.push_back(' ');
	}
	context += '^';
	for (int i = 0; i < length-1 and i+column+1 < maxlen; i++)
		context += '~';
	context += '\n';

	return *this;
}

messenger &messenger::operator() (std::string file, long line, long column, long length)
{
	this->file = file;
	this->line = line;
	this->column = column;
	this->length = length;
	this->text.clear();
	this->context.clear();

	return *this;
}

messenger::operator message()
{
	message result;
	result.type = type;
	result.file = file;
	result.line = line;
	result.column = column;
	result.length = length;
	result.text = text.str();
	result.context = context;
	return result;
}

std::ostream &operator<<(std::ostream &os, const messenger &msg)
{
	if (msg.file.size() > 0)
		os << msg.file << ":";

	if (msg.line >= 0)
		os << msg.line << ":";

	if (msg.column >= 0)
		os << msg.column << " ";

	switch (msg.type)
	{
		case message::fail: os << "fail: "; break;
		case message::error: os << "error: "; break;
		case message::warning: os << "warning: "; break;
		default: os << "note: "; break;
	}

	os << msg.text.str() << std::endl;
	if (msg.context.size() > 0)
		os << msg.context;
	return os;
}

}
