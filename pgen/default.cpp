#include <pgen/default.h>

#include <sstream>
#include <iostream>

namespace pgen
{

/********************** STEM **********************/

stem::stem(int index, bool keep) : symbol_t(keep)
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

symbol_t *stem::clone(int rule_offset) const
{
	return new stem(index + rule_offset, keep);
}

std::string stem::emit() const
{
	std::stringstream result;
	result << "stem(" << index << ", " << (keep ? "true" : "false") << ")";
	return result.str();
}

character_match::character_match()
{
}

character_match::character_match(std::string match, bool keep) : symbol_t(keep)
{
	invert = false;

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
			if (not range and match[i] == '-' and ranges.size() > 0 and ranges.back().first == ranges.back().second)
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

character_match::~character_match()
{
}

std::string character_match::escape(char c) const
{
	switch (c) {
		case '\0': return "\\0";
		case '\a': return "\\a";
		case '\b': return "\\b";
		case '\f': return "\\f";
		case '\n': return "\\n";
		case '\r': return "\\r";
		case '\t': return "\\t";
		case '\v': return "\\v";
		case '^': return "\\^";
		default: return std::string(1, c);
	}
}

char character_match::unescape(char c) const
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
	case '^': return '^';
	default: return c;
	}
}

std::string character_match::name() const
{
	std::string result;
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
	return result;
}

parsing character_match::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = CHARACTER;
	
	char c = lexer.get();
	++result.tree.end;
	
	bool match = false;
	for (int i = 0; i < (int)ranges.size() and not match; i++)
		if (c >= ranges[i].first && c <= ranges[i].second)
			match = true;

	if (match == invert)
		result.msgs.push_back(error(lexer, result.tree) << "expected " << name() << " but found '" << escape(c) << "'.");

	return result;
}

symbol_t *character_match::clone(int rule_offset) const
{
	character_match *result = new character_match();
	result->ranges = ranges;
	result->invert = invert;
	result->keep = keep;
	return result;
}

std::string character_match::emit() const
{
	std::stringstream result;
	std::string txt = name();
	std::string esc;
	for (int i = 0; i < (int)txt.size(); i++)
	{
		if (txt[i] == '\\')
			esc.push_back('\\');
		esc.push_back(txt[i]);
	}
	result << "character_match(\"" << esc << "\", " << (keep ? "true" : "false") << ")";
	return result.str();
}


keyword::keyword(std::string value, bool keep) : symbol_t(keep)
{
	this->value = value;
}

keyword::~keyword()
{
}

parsing keyword::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = KEYWORD;
	for (int i = 0; i < (int)value.size(); i++)
	{
		char c = lexer.get();
		++result.tree.end;

		if (c != value[i]) {
			result.msgs.push_back(error(lexer, result.tree) << "expected \"" << value << "\".");
			return result;
		}
	}

	return result;
}

symbol_t *keyword::clone(int rule_offset) const
{
	return new keyword(value);
}

std::string keyword::emit() const
{
	std::stringstream result;
	result << "keyword(\"" << value << "\", " << (keep ? "true" : "false") << ")";
	return result.str();
}

instance::instance(bool keep) : symbol_t(keep)
{
}

instance::~instance()
{
}

parsing instance::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = INSTANCE;
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
		result.msgs.push_back(error(lexer, result.tree) << "expected instance.");

	return result;
}

symbol_t *instance::clone(int rule_offset) const
{
	return new instance();
}

std::string instance::emit() const
{
	std::stringstream result;
	result << "instance(" << (keep ? "true" : "false") << ")";
	return result.str();
}

text::text(bool keep) : symbol_t(keep)
{
}

text::~text()
{
}

parsing text::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = TEXT;

	char c = lexer.get();
	++result.tree.end;
	if (c != '\"')
	{
		lexer.unget();
		result.msgs.push_back(error(lexer, result.tree) << "expected text.");
		return result;
	}

	do {
		if (c == '\\') {
			c = lexer.get();
			++result.tree.end;
		}

		if (c != 0) {
			c = lexer.get();
			++result.tree.end;
		}
	} while (c != '\"' and c != 0);

	if (c == 0)
		result.msgs.push_back(error(lexer, result.tree) << "dangling text.");

	return result;
}

symbol_t *text::clone(int rule_offset) const
{
	return new text();
}

std::string text::emit() const
{
	std::stringstream result;
	result << "text(" << (keep ? "true" : "false") << ")";
	return result.str();
}

whitespace::whitespace(bool brk, bool keep) : symbol_t(keep)
{
	this->brk = brk;
}

whitespace::~whitespace()
{
}

parsing whitespace::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = WHITESPACE;

	char c = lexer.get();
	while ((c == ' ' or
	       c == '\t' or
	       (brk and 
	         (c == '\n' or
	         c == '\r')
	       )) and c != 0)
	{
		++result.tree.end;
		c = lexer.get();
	}

	if (c != 0)
		lexer.unget();

	return result;
}

symbol_t *whitespace::clone(int rule_offset) const
{
	return new whitespace();
}

std::string whitespace::emit() const
{
	std::stringstream result;
	result << "whitespace(" << (brk ? "true" : "false") << ", " << (keep ? "true" : "false") << ")";
	return result.str();
}

integer::integer(int base, bool keep) : symbol_t(keep)
{
	this->base = base;
}

integer::~integer()
{
}

parsing integer::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = INTEGER;

	char c = lexer.get();
	while (((c >= '0' and c < ('0' + std::min(base, 10)))
	     or (c >= 'a' and c < ('a' + std::max(base-10, 0)))
	     or (c >= 'A' and c < ('A' + std::max(base-10, 0))))
	   and c != 0)
	{
		++result.tree.end;
		c = lexer.get();
	}

	if (c != 0)
		lexer.unget();

	if (result.tree.end - result.tree.begin == 0) {
		result.msgs.push_back(error(lexer, result.tree) << "expected integer.");
	}

	return result;
}

symbol_t *integer::clone(int rule_offset) const
{
	return new integer(base);
}

std::string integer::emit() const
{
	std::stringstream result;
	result << "integer(" << base << ", " << (keep ? "true" : "false") << ")";
	return result.str();
}

character_class::character_class(bool keep) : symbol_t(keep)
{
}

character_class::~character_class()
{
}

parsing character_class::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = CHARACTER_CLASS;

	char c = lexer.get();
	++result.tree.end;
	if (c != '[')
	{
		lexer.unget();
		result.msgs.push_back(error(lexer, result.tree) << "expected character_class.");
		return result;
	}

	c = lexer.get();
	++result.tree.end;
	bool escape = false;
	while ((escape or c != ']') and c != 0)
	{
		if (escape)
			escape = false;
		else if (c == '\\')
			escape = true;

		c = lexer.get();
		++result.tree.end;
	}

	if (c == 0)
		result.msgs.push_back(error(lexer, result.tree) << "dangling character_class.");

	return result;
}

symbol_t *character_class::clone(int rule_offset) const
{
	return new character_class();
}

std::string character_class::emit() const
{
	std::stringstream result;
	result << "character_class(" << (keep ? "true" : "false") << ")";
	return result.str();
}

character::character(uint8_t from, uint8_t to)
{
	this->from = from;
	this->to = to;
}

character::~character()
{
}

bool character::match(uint8_t c) const
{
	return c >= from and c <= to;
}

regular_expression::regular_expression(std::string value, bool keep) : symbol_t(keep)
{
	this->value = value;

	segment seg;
	seg.to.push_back(-1);
	if (load(value.begin(), value.end(), seg)) {
		start = seg.from;
		for (int i = 0; i < (int)seg.to.size(); i++) {
			if (seg.to[i] >= 0) {
				expr[seg.to[i]].branch.push_back(-1);
			} else {
				start.push_back(-1);
			}
		}
	}
	
	/*std::cout << "Parsing \"" << value << "\"" << std::endl;
	std::cout << "Start: [";
	for (int i = 0; i < (int)start.size(); i++) {
		if (i != 0) {
			std::cout << ", ";
		}
		std::cout << start[i];
	}
	std::cout << "]" << std::endl;

	for (int i = 0; i < (int)expr.size(); i++) {
		std::cout << i << ": " << "'" << expr[i].from << "-" << expr[i].to << "' -> [";
		for (int j = 0; j < (int)expr[i].branch.size(); j++) {
			if (j != 0) {
				std::cout << ", ";
			}
			std::cout << expr[i].branch[j];
		}
		std::cout << "]" << std::endl;
	}*/
}

regular_expression::~regular_expression()
{
}

char regular_expression::unescape(std::string::iterator &i) const
{
	switch (*i)
	{
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	}

	if (*i == 'x') {
		std::string value;
		i++;
		value += *i;
		char c = *(i+1);
		if ((c >= '0' and c <= '9') or (c >= 'A' and c <= 'F') or (c >= 'a' and c <= 'f')) {
			i++;
			value += *i;
		}
		return stoi(value, 0, 16);
	} else if (*i >= '0' and *i <= '7') {
		std::string value;
		value += *i;
		char c = *(i+1);
		if (c >= '0' and c <= '7') {
			i++;
			value += *i;
		}

		c = *(i+1);
		if (c >= '0' and c <= '7') {
			i++;
			value += *i;
		}

		return stoi(value, 0, 8);
	} else {
		return *i;
	}
}

bool regular_expression::load(std::string::iterator begin, std::string::iterator end, segment &result) {
	std::vector<int16_t> from, to;
	
	result.i = begin;

	while (result.i != end) {
		if (*result.i == '\\') {
			result.i++;
			char c = unescape(result.i);
			from.push_back(expr.size());
			to.push_back(expr.size());
			expr.push_back(character(c, c));
		} else if (*result.i == '[') {
			result.i++;

			bool invert = false;
			if (*result.i == '^') {
				invert = true;
				result.i++;
			}

			std::vector<std::pair<char, char> > ranges;
			bool first_char = true;
			while (result.i != end and *result.i != ']') {
				if (*result.i == '\\') {
					result.i++;
					char c = unescape(result.i);
					ranges.push_back(std::pair<char, char>(c, c));
				} else if (*result.i == '-' and not first_char) {
					result.i++;
					if (*result.i == '\\') {
						result.i++;
						ranges.back().second = unescape(result.i);
					} else {
						ranges.back().second = *result.i;
					}
				} else {
					ranges.push_back(std::pair<char, char>(*result.i, *result.i));
				}

				first_char = false;
				result.i++;
			}

			if (invert) {
				char start = 0;
				sort(ranges.begin(), ranges.end());
				for (std::vector<std::pair<char, char> >::iterator j = ranges.begin(); j != ranges.end(); j++) {
					from.push_back(expr.size());
					to.push_back(expr.size());
					expr.push_back(character(start, j->first-1));
					start = j->second+1;
				}

				from.push_back(expr.size());
				to.push_back(expr.size());
				expr.push_back(character(start, 255));
			} else {
				for (std::vector<std::pair<char, char> >::iterator j = ranges.begin(); j != ranges.end(); j++) {
					from.push_back(expr.size());
					to.push_back(expr.size());
					expr.push_back(character(j->first, j->second));
				}
			}

			if (*result.i != ']') {
				std::cout << "internal: expected ']', found '" << (result.i == end ? "EOF" : std::string(1, *result.i).c_str()) << "'" << std::endl;
				return false;
			}
		} else if (*result.i == '(') {
			while (result.i != end and (*result.i == '|' or *result.i == '(')) {
				result.i++;
				segment seg;
				if (!load(result.i, end, seg)) {
					return false;
				}
				result.i = seg.i;
				from.insert(from.end(), seg.from.begin(), seg.from.end());
				to.insert(to.end(), seg.to.begin(), seg.to.end());
			}

			if (*result.i != ')') {
				std::cout << "internal: expected '(', '|', or ')', found '" << (result.i == end ? "EOF" : std::string(1, *result.i).c_str()) << "'" << std::endl;
				return false;
			}
		} else if (*result.i == '.') {
			from.push_back(expr.size());
			to.push_back(expr.size());
			expr.push_back(character(0, 255));
		} else if (*result.i == ')' || *result.i == '|') {
			return true;
		} else {
			from.push_back(expr.size());
			to.push_back(expr.size());
			expr.push_back(character(*result.i, *result.i));
		}

		result.i++;

		if (result.from.size() == 0) {
			result.from = from;
		}

		for (int j = 0; j < (int)result.to.size(); j++) {
			if (result.to[j] >= 0) {
				expr[result.to[j]].branch.insert(expr[result.to[j]].branch.end(), from.begin(), from.end());
			}
		}

		if (*result.i == '*') {
			for (int j = 0; j < (int)to.size(); j++) {
				expr[to[j]].branch.insert(expr[to[j]].branch.end(), from.begin(), from.end());
			}
			result.to.insert(result.to.end(), to.begin(), to.end());
			result.i++;
		} else if (*result.i == '+') {
			for (int j = 0; j < (int)to.size(); j++) {
				expr[to[j]].branch.insert(expr[to[j]].branch.end(), from.begin(), from.end());
			}
			result.to = to;
			result.i++;
		} else if (*result.i == '?') {
			result.to.insert(result.to.end(), to.begin(), to.end());
			result.i++;
		} else {
			result.to = to;
		}

		from.clear();
		to.clear();
	}

	return true;
}

parsing regular_expression::parse(lexer_t &lexer) const
{
	parsing result(lexer.offset);
	result.tree.type = REGULAR_EXPRESSION;
	result.tree.end = -1;

	std::vector<int16_t> tokens = start;
	for (int i = (int)tokens.size()-1; i >= 0; i--) {
		if (tokens[i] < 0) {
			result.tree.end = lexer.offset;
			tokens.erase(tokens.begin() + i);
		}
	}

	while (true) {
		char c = lexer.get();
	
		for (int i = (int)tokens.size()-1; i >= 0; i--) {
			if (expr[tokens[i]].match(c)) {
				for (int j = (int)expr[tokens[i]].branch.size()-1; j > 0; j--) {
					if (expr[tokens[i]].branch[j] < 0) {
						result.tree.end = lexer.offset;
					} else {
						tokens.push_back(expr[tokens[i]].branch[j]);
					}
				}

				if (expr[tokens[i]].branch.size() == 0 or expr[tokens[i]].branch[0] < 0) {
					result.tree.end = lexer.offset;
					tokens.erase(tokens.begin()+i);
				} else {
					tokens[i] = expr[tokens[i]].branch[0];
				}
			} else {
				tokens.erase(tokens.begin() + i);
			}
		}

		if (tokens.size() == 0) {
			if (result.tree.end < 0) {
				result.tree.end = lexer.offset;
				result.msgs.push_back(error(lexer, result.tree) << "expected \"" << value << "\".");
			} else {
				lexer.moveto(result.tree.end);
			}
			return result;
		}
	}
	
	return result;
}

symbol_t *regular_expression::clone(int rule_offset) const
{
	return new regular_expression(value);
}

std::string regular_expression::emit() const
{
	std::stringstream result;
	result << "regular_expression(\"";
	for (std::string::const_iterator i = value.begin(); i != value.end(); i++) {
		if (*i == '\\') {
			result << "\\\\";
		} else if (*i == '"') {
			result << "\\\"";
		} else if (*i < 32 or *i >= 127) {
			result << "\\" << std::oct << (int)*i << std::dec;
		} else {
			result << *i;
		}
	}
	result << "\", " << (keep ? "true" : "false") << ")";
	return result.str();
}

}
