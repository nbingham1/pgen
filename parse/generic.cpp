/*
 * generic.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: nbingham
 */

#include <parse/generic.h>
#include <parse/default.h>

namespace parse
{

generic_t::segment::segment() : start(), end()
{
	skip = false;
}

generic_t::segment::~segment()
{
}

generic_t::segment &generic_t::segment::sequence(const generic_t::segment &s)
{
	if (end.size() > 0) {
		for (int i = 0; i < (int)end.size(); i++)
			for (int j = 0; j < (int)s.start.size(); j++)
				end[i].link(s.start[j]);

		if (skip)
			start.insert(start.end(), s.start.begin(), s.start.end());

		if (s.skip)
			end.insert(end.end(), s.end.begin(), s.end.end());
		else
			end = s.end;

		skip = false;
	} else {
		start = s.start;
		end = s.end;
		skip = s.skip;
	}

	msgs.insert(msgs.end(), s.msgs.begin(), s.msgs.end());

	return *this;
}

generic_t::segment &generic_t::segment::parallel(const generic_t::segment &s)
{
	msgs.insert(msgs.end(), s.msgs.begin(), s.msgs.end());
	start.insert(start.end(), s.start.begin(), s.start.end());
	end.insert(end.end(), s.end.begin(), s.end.end());
	if (s.skip)
		skip = true;
	return *this;
}

generic_t::segment generic_t::load_term(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token)
{
	segment result;
	if (token.tokens.size() > 1 and token.tokens[1].type == "choice") {
		result = load_choice(definitions, lexer, token.tokens[1]);
	} else if (token.tokens.size() > 0) {
		std::vector<token_t>::const_iterator i = token.tokens.begin();
		iterator term = end();
		if (i->type == "text") {
			std::string word = lexer.read(i->begin, i->end);
			term = insert(new keyword(word.substr(1, word.size()-2)));
		} else if (i->type == "character_class") {
			std::string word = lexer.read(i->begin, i->end);
			term = insert(new character(word.substr(1, word.size()-2)));
		} else if (i->type == "instance") {
			std::string name = lexer.read(i->begin, i->end);
			if (name == "instance")
				term = insert(new instance());
			else if (name == "text")
				term = insert(new text());
			else if (name == "_")
				term = insert(new whitespace());
			else if (name == "integer")
				term = insert(new integer());
			else if (name == "character_class")
				term = insert(new character_class());
			else
			{
				std::map<std::string, int>::iterator definition = definitions.find(name);
				if (definition != definitions.end())
					term = insert(new stem(definition->second));
				else
				{
					int index = rules.size();
					rules.push_back(rule(name));
					definitions.insert(std::pair<std::string, int>(name, index));
					term = insert(new stem(index));
				}
			}
		} else {
			result.msgs.push_back(message(message::fail, "invalid term type '" + i->type + "'.", lexer, true, i->begin, i->end));
		}

		if (term != end()) {
			result.start.push_back(term);
			result.end.push_back(term);
		}
	}

	if (token.tokens.size() > 0)
	{
		std::string attr = lexer.read(token.tokens.back().begin, token.tokens.back().end);
		if (attr == "*" or attr == "+") {
			link_iterator i, j;
			for (link_iterator i = result.end.begin(); i != result.end.end(); i++)
				for (link_iterator j = result.start.begin(); j != result.start.end(); j++)
					i->link(*j);
		}

		if (attr == "?" or attr == "*")
			result.skip = true;
	}

	return result;
}

generic_t::segment generic_t::load_sequence(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token)
{
	segment result;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == "term")
			result.sequence(load_term(definitions, lexer, *i));
		else if (i->type == "sequence")
			result.sequence(load_sequence(definitions, lexer, *i));
	}

	return result;
}

generic_t::segment generic_t::load_choice(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token)
{
	segment result;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == "term")
			result.parallel(load_term(definitions, lexer, *i));
		else if (i->type == "sequence")
			result.parallel(load_sequence(definitions, lexer, *i));
		else if (i->type == "choice")
			result.parallel(load_choice(definitions, lexer, *i));
	}

	return result;
}

void generic_t::load_definition(std::map<std::string, int> &definitions, lexer_t &lexer, const token_t &token)
{
	if (token.tokens.size() == 4) {
		std::string name = lexer.read(token.tokens[0].begin, token.tokens[0].end);
		std::map<std::string, int>::iterator result = definitions.lower_bound(name);
		if (result == definitions.end() || result->first != name) {
			result = definitions.insert(result, std::pair<std::string, int>(name, (int)rules.size()));
			rules.push_back(rule(name));
		}
		
		if (rules[result->second].start.size() == 0) {
			segment seg = load_choice(definitions, lexer, token.tokens[2]);
			for (int i = 0; i < (int)seg.msgs.size(); i++)
				seg.msgs[i].emit();

			for (int i = 0; i < (int)seg.start.size(); i++)
				rules[result->second].start.push_back(seg.start[i]);
			if (seg.skip)
				rules[result->second].start.push_back(end());

			for (link_iterator i = seg.end.begin(); i != seg.end.end(); i++)
				i->link(end());
		} else {
			message err(message::error, "multiple definitions for '" + name + "'.", lexer, true, token.begin, token.end);
			err.emit();
		}
	} else {
		message err(message::fail, "incorrect format for 'definition' should have been caught by the parser", lexer, true, token.begin, token.end);
		err.emit();
	}
}

void generic_t::load(lexer_t &lexer, const token_t &token)
{
	std::map<std::string, int> definitions;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == "definition")
			load_definition(definitions, lexer, *i);
		else if (i->type == "grammar")
			load(lexer, *i);
	}

	for (int i = 0; i < (int)rules.size(); i++)
		if (rules[i].start.size() == 0) {
			message err(message::error, "definition '" + rules[i].name + "' not found.", lexer, false, -1, -1);
			err.emit();
		}
}

void generic_t::save(std::string space, std::string name, std::ostream &header, std::ostream &source)
{
	using std::endl;
	
	header << "#pragma once" << endl << endl;
	header << "#include <parse/grammar.h>" << endl << endl;
	if (space.size() > 0) {
		header << "namespace " << space << endl;
		header << "{" << endl << endl;
	}
	header << "struct " << name << "_t : parse::grammar_t" << endl;
	header << "{" << endl;
	header << "\t" << name << "_t();" << endl;
	header << "\t~" << name << "_t();" << endl;
	header << "};" << endl << endl;
	if (space.size() > 0) {
		header << "}" << endl << endl;
	}	

	source << "#include \"" << name << ".h\"" << endl << endl;
	source << "#include <parse/default.h>" << endl << endl;

	if (space.size() > 0) {
		source << "namespace " << space << endl;
		source << "{" << endl << endl;
	}

	source << name << "_t::" << name << "_t()" << endl;
	source << "{" << endl;

	for (int i = 0; i < (int)rules.size(); i++)
		source << "\trules.push_back(rule(\"" << rules[i].name << "\", " << (rules[i].keep ? "true" : "false") << "));" << endl;
	source << endl;

	source << "\titerator n[" << size() << "];" << endl; 
	int index = 0;
	std::map<const node*, int> indices;
	for (iterator i = begin(); i != end(); i++)
	{
		indices.insert(std::pair<const node*, int>(i.loc, index));
		source << "\tn[" << index++ << "] = insert(new " << i->emit() << ");" << endl;
	}
	source << endl;

	index = 0;
	for (iterator i = begin(); i != end(); i++)
	{
		for (link_iterator j = i.next().begin(); j != i.next().end(); j++)
		{
			if (not *j)
				source << "\tn[" << index << "].link(end());" << endl;
			else
			{
				std::map<const node*, int>::iterator nextindex = indices.find(j->loc);
				if (nextindex != indices.end())
					source << "\tn[" << index << "].link(n[" << nextindex->second << "]);" << endl;
				else
					printf("link not found from %d\n", index);
			}
		}
		++index;
	}
	source << endl;

	for (int i = 0; i < (int)rules.size(); i++)
	{
		for (const_link_iterator j = rules[i].start.begin(); j != rules[i].start.end(); j++)
		{
			if (not *j)
				source << "\trules[" << i << "].start.push_back(end());" << endl;
			else
			{
				std::map<const node*, int>::iterator nextindex = indices.find(j->loc);
				if (nextindex != indices.end())
					source << "\trules[" << i << "].start.push_back(n[" << nextindex->second << "]);" << endl;
				else
					printf("link not found from rule %d\n", i);
			}
		}
	}

	source << "}" << endl << endl;

	source << name << "_t::~" << name << "_t()" << endl;
	source << "{" << endl;
	source << "}" << endl << endl;
	
	if (space.size() > 0) {
		source << "}" << endl << endl;
	}
}

}
