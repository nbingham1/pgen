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

uint8_t decode_char(char c, int base) {
	uint8_t value = 255;
	if (c >= '0' and c <= '9') {
		value = c - '0';
	} else if (c >= 'A' and c <= 'Z') {
		value = 10 + c - 'A';
	} else if (c >= 'a' and c <= 'z') {
		value = 10 + c - 'a';
	}

	if (value >= base) {
		std::cout << "internal: cannot decode '" << c << "' to base " << base << " integer" << std::endl;
	}

	return value;
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
		uint8_t value = 0;
		i++;
		value += decode_char(*i, 16);
		char c = *(i+1);
		if ((c >= '0' and c <= '9') or (c >= 'A' and c <= 'F') or (c >= 'a' and c <= 'f')) {
			value = (value << 4) + decode_char(*++i, 16);
		}
		return value;
	} else if (*i >= '0' and *i <= '7') {
		uint8_t value = decode_char(*i, 8);
		char c = *(i+1);
		if (c >= '0' and c <= '7') {
			value += decode_char(*++i, 8);
		}
		c = *(i+1);
		if (c >= '0' and c <= '7') {
			value += decode_char(*++i, 8);
		}
		return value;
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
