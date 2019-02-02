/*
 * generic.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: nbingham
 */

#include <parse/generic.h>
#include <parse/default.h>
#include <parse/peg.h>

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

generic_t::segment generic_t::load_term(lexer_t &lexer, const token_t &token)
{
	bool keep = true;
	segment result;
	std::vector<token_t>::const_iterator i = token.tokens.begin();
	std::string word;

	if (i != token.tokens.end()) {
		word = lexer.read(i->begin, i->end);
		if (word == "~") {
			keep = false;
			i++;
		}
	}

	if (i != token.tokens.end()) {
		word = lexer.read(i->begin, i->end);
		iterator term = end();
		if (word == "(") {
			i++;
			if (i->type == "peg::choice") {
				result = load_choice(lexer, *i);
			}
		} else if (i->type == "text") {
			term = insert(new keyword(word.substr(1, word.size()-2), keep));
		} else if (i->type == "character_class") {
			term = insert(new character(word.substr(1, word.size()-2), keep));
		} else if (i->type == "peg::name") {
			if (word == "instance")
				term = insert(new instance(keep));
			else if (word == "text")
				term = insert(new text(keep));
			else if (word == "_")
				term = insert(new whitespace(true));
			else if (word == "__")
				term = insert(new whitespace(false));
			else if (word == "integer")
				term = insert(new integer(keep));
			else if (word == "character_class")
				term = insert(new character_class(keep));
			else
			{
				size_t space = word.rfind("::");
				if (space == std::string::npos)
					word = lexer.basename + "::" + word;

				std::map<std::string, int>::iterator definition = definitions.find(word);
				if (definition != definitions.end())
					term = insert(new stem(definition->second, keep));
				else
				{
					int index = rules.size();
					rules.push_back(rule(word));
					definitions.insert(std::pair<std::string, int>(word, index));
					term = insert(new stem(index, keep));
				}
			}
		} else {
			result.msgs.push_back(message(message::fail, "unrecognied term type '" + i->type + "'.", lexer, true, i->begin, i->end));
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

generic_t::segment generic_t::load_sequence(lexer_t &lexer, const token_t &token)
{
	segment result;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == "peg::term")
			result.sequence(load_term(lexer, *i));
		else if (i->type == "peg::sequence")
			result.sequence(load_sequence(lexer, *i));
	}

	return result;
}

generic_t::segment generic_t::load_choice(lexer_t &lexer, const token_t &token)
{
	segment result;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == "peg::term")
			result.parallel(load_term(lexer, *i));
		else if (i->type == "peg::sequence")
			result.parallel(load_sequence(lexer, *i));
		else if (i->type == "peg::choice")
			result.parallel(load_choice(lexer, *i));
	}

	return result;
}

void generic_t::load_definition(lexer_t &lexer, const token_t &token)
{
	if (token.tokens.size() == 4) {
		std::string name = lexer.basename + "::" + lexer.read(token.tokens[0].begin, token.tokens[0].end);

		bool atomic = true;
		std::string type = lexer.read(token.tokens[1].begin, token.tokens[1].end);
		if (type == "@=")
			atomic = false;

		std::map<std::string, int>::iterator result = definitions.lower_bound(name);
		if (result == definitions.end() || result->first != name) {
			result = definitions.insert(result, std::pair<std::string, int>(name, (int)rules.size()));
			rules.push_back(rule(name, atomic));
		} else {
			rules[result->second].atomic = atomic;
		}
		
		if (rules[result->second].start.size() == 0) {
			segment seg = load_choice(lexer, token.tokens[2]);
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

void generic_t::load_import(lexer_t &lexer, const token_t &token)
{
	if (token.tokens.size() == 3) {
		std::string name = lexer.read(token.tokens[1].begin, token.tokens[1].end);
		name = name.substr(1, name.size()-2);
		lexer_t sublexer;
		if (sublexer.open(name))
		{
			std::vector<std::string>::iterator loc = std::lower_bound(imports.begin(), imports.end(), sublexer.basename);
			if (loc == imports.end() || *loc != sublexer.basename) {
				loc = imports.insert(loc, sublexer.basename);

				peg_t peg;
				parsing result = peg.parse(sublexer);
				if (result.msgs.size() == 0) {
					load_grammar(sublexer, result.tree);
				} else {
					message err(message::note, "imported from '" + name + "':", lexer, true, token.begin, token.end);
					err.emit();
					for (int i = 0; i < (int)result.msgs.size(); i++)
						result.msgs[i].emit();
				}
			}

			sublexer.close();
		}
		else
		{
			message err(message::error, "file '" + name + "' not found.");
			err.emit();
		}
	} else {
		message err(message::fail, "incorrect format for 'import' should have been caught by the parser", lexer, true, token.begin, token.end);
		err.emit();
	}
}

void generic_t::load_grammar(lexer_t &lexer, const token_t &token)
{
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == "peg::definition")
				load_definition(lexer, *i);
		else if (i->type == "peg::import")
			load_import(lexer, *i);
		else if (i->type == "peg::grammar")
			load_grammar(lexer, *i);
		else if (i->type != "character") {
			message err(message::fail, "unrecognized grammar type '" + i->type + "'.", lexer, true, token.begin, token.end);
			err.emit();
		}
	}
}

void generic_t::load(std::string filename)
{
	lexer_t lexer;
	if (lexer.open(filename))
	{
		std::vector<std::string>::iterator loc = std::lower_bound(imports.begin(), imports.end(), lexer.basename);
		if (loc == imports.end() || *loc != lexer.basename) {
			loc = imports.insert(loc, lexer.basename);

			peg_t peg;
			parsing result = peg.parse(lexer);
			if (result.msgs.size() == 0) {
				load_grammar(lexer, result.tree);

				for (int i = 0; i < (int)rules.size(); i++)
					if (rules[i].start.size() == 0) {
						message err(message::warning, "definition '" + rules[i].name + "' not found.");
						err.emit();
					}
			} else
				for (int i = 0; i < (int)result.msgs.size(); i++)
					result.msgs[i].emit();
		}

		lexer.close();
	}
	else
	{
		message err(message::error, "file '" + filename + "' not found.");
		err.emit();
	}
}

}
