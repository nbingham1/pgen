#include <parse/default.h>

namespace parse
{

/********************** STEM **********************/

stem::stem(int index, bool keep) : grammar_t::symbol(keep)
{
	this->index = index;
}

stem::~stem()
{
}

parsing stem::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.stem = index;
	return result;
}

character::character(std::string match)
{
	invert = false;
	keep = false;

	if (match.size() > 0)
	{
		int i = 0;
		if (match[i] == '^')
		{
			invert = true;
			++i;
		}

		bool range = false;
		while (i < (int)match.size())
		{
			if (not range and match[i] == '-')
				range = true;
			else
			{
				char c;
				if (match[i] == '\\')
				{
					if (++i < (int)match.size())
						c = unescape(match[i]);
					else
						c = '\0';
				}
				else
					c = match[i];

				if (range)
				{
					ranges.back().second = c;
					range = false;
				}
				else
					ranges.push_back(std::pair<char, char>(c, c));
			}

			++i;
		}
	}
}

character::~character()
{
}

std::string character::escape(char c) const
{
	switch (c) {
		case '\a': return "\\a";
		case '\b': return "\\b";
		case '\f': return "\\f";
		case '\n': return "\\n";
		case '\r': return "\\r";
		case '\t': return "\\t";
		case '\v': return "\\v";
		case '\0': return "\\0";
		default: return std::string(1, c);
	}
}

char character::unescape(char c) const
{
	switch (c)
	{
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	case '0': return '\0';
	default: return c;
	}
}

std::string character::name() const
{
	std::string result;
	result.push_back('[');
	if (invert)
		result.push_back('^');
	for (int i = 0; i < (int)ranges.size(); i++)
	{
		if (ranges[i].first == ranges[i].second)
			result += escape(ranges[i].first);
		else {
			result += escape(ranges[i].first);
			result.push_back('-');
			result += escape(ranges[i].second);
		}
	}
	result.push_back(']');
	return result;
}

parsing character::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = "character";
	
	char c = lexer.get();
	++result.tree.end;
	
	bool match = false;
	for (int i = 0; i < (int)ranges.size() and not match; i++)
		if (c >= ranges[i].first && c <= ranges[i].second)
			match = true;

	if (match == invert)
		result.msgs.push_back(message(message::error, std::string("expected ") + name() + " but found '" + escape(c) + "'.", lexer, true, result.tree.begin, result.tree.end));
	
	return result;
}

keyword::keyword(std::string value)
{
	this->keep = true;
	this->value = value;
}

keyword::~keyword()
{
}

parsing keyword::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = "keyword";
	for (int i = 0; i < (int)value.size(); i++)
	{
		char c = lexer.get();
		++result.tree.end;

		if (c != value[i]) {
			result.msgs.push_back(message(message::error, std::string("expected \"") + value + "\".", lexer, true, result.tree.begin, result.tree.end));
			return result;
		}
	}

	return result;
}

instance::instance()
{
	this->keep = true;
}

instance::~instance()
{
}

parsing instance::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = "instance";
	bool first = true;

	char c = lexer.get();
	while (((c >= 'a' and c <= 'z') or
	       (c >= 'A' and c <= 'Z') or
	       (c >= '0' and c <= '9' and not first) or
	       c == '_') and c != 0)
	{
		++result.tree.end;
		c = lexer.get();
		first = false;
	}

	if (c != 0)
		lexer.unget();

	if (result.tree.end - result.tree.begin == 0)
		result.msgs.push_back(message(message::error, "expected instance.", lexer, true, result.tree.begin, result.tree.end));

	return result;
}

text::text()
{
	this->keep = true;
}

text::~text()
{
}

parsing text::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = "text";

	char c = lexer.get();
	++result.tree.end;
	if (c != '\"')
	{
		lexer.unget();
		result.msgs.push_back(message(message::error, "expected text.", lexer, true, result.tree.begin, result.tree.end));
		return result;
	}

	c = lexer.get();
	++result.tree.end;
	bool escape = false;
	while ((escape or c != '\"') and c != 0)
	{
		if (escape)
			escape = false;
		else if (c == '\\')
			escape = true;

		c = lexer.get();
		++result.tree.end;
	}

	if (c == 0)
		result.msgs.push_back(message(message::error, "dangling text.", lexer, true, result.tree.begin, result.tree.end));

	return result;
}

whitespace::whitespace()
{
	this->keep = false;
}

whitespace::~whitespace()
{
}

parsing whitespace::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = "whitespace";

	char c = lexer.get();
	while ((c == ' ' or
	       c == '\t' or
	       c == '\n' or
	       c == '\r') and c != 0)
	{
		++result.tree.end;
		c = lexer.get();
	}

	if (c != 0)
		lexer.unget();

	return result;
}

integer::integer()
{
	this->keep = true;
}

integer::~integer()
{
}

parsing integer::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = "integer";

	char c = lexer.get();
	while ((c >= '0' and c <= '9') and c != 0)
	{
		++result.tree.end;
		c = lexer.get();
	}

	if (c != 0)
		lexer.unget();

	if (result.tree.end - result.tree.begin == 0)
		result.msgs.push_back(message(message::error, "expected integer.", lexer, true, result.tree.begin, result.tree.end));

	return result;
}


}
