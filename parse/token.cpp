#include <parse/token.h>

namespace parse
{

token_t::token_t(intptr_t begin, intptr_t end)
{
	this->begin = begin;
	this->end = end >= 0 ? end : begin;
}

token_t::~token_t()
{
}

token_t::operator bool()
{
	return begin >= 0 and end >= 0 and begin != end;
}

void token_t::append(const token_t &t)
{
	if (begin < 0 or (t.begin >= 0 and begin > t.begin))
		begin = t.begin;
	if (end < 0 or (t.end >= 0 and end < t.end))
		end = t.end;
	tokens.push_back(t);
}

void token_t::extend(const token_t &t)
{
	if (begin < 0 or (t.begin >= 0 and begin > t.begin))
		begin = t.begin;
	if (end < 0 or (t.end >= 0 and end < t.end))
		end = t.end;
	tokens.insert(tokens.end(), t.tokens.begin(), t.tokens.end());
}

void token_t::skip(const token_t &t)
{
	if (begin < 0 or (t.begin >= 0 and begin > t.begin))
		begin = t.begin;
	if (end < 0 or (t.end >= 0 and end < t.end))
		end = t.end;
}

void token_t::clear()
{
	tokens.clear();
	end = begin;
}

void token_t::emit(lexer_t &lexer, std::string tab)
{
	if (tokens.size() == 0)
		printf("%s%s:%ld-%ld: \"%s\"\n", tab.c_str(), type.c_str(), begin, end, lexer.read(begin, end).c_str());
	else if (tokens.size() == 1)
	{
		printf("%s%s", tab.c_str(), type.c_str());
		tokens[0].emit(lexer, tab);
	}
	else
	{
		printf("%s%s:%ld-%ld: \"%s\"\n%s{\n", tab.c_str(), type.c_str(), begin, end, lexer.read(begin, end).c_str(), tab.c_str());
		for (int i = 0; i < (int)tokens.size(); i++)
			tokens[i].emit(lexer, tab+" ");
		printf("%s}\n", tab.c_str());
	}
}

}
