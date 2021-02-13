/*
 * generic.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: nbingham
 */

#include <pgen/generic.h>
#include <pgen/default.h>
#include <pgen/peg.h>

namespace pgen
{

segment_t::segment_t()
{
	skip = false;
}

segment_t::~segment_t()
{
}

segment_t &segment_t::sequence(const segment_t &s)
{
	if (end.size() > 0) {
		for (int i = 0; i < (int)end.size(); i++)
			for (int j = 0; j < (int)s.start.size(); j++)
				end[i]->next.push_back(s.start[j]);

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

segment_t &segment_t::parallel(const segment_t &s)
{
	msgs.insert(msgs.end(), s.msgs.begin(), s.msgs.end());
	start.insert(start.end(), s.start.begin(), s.start.end());
	end.insert(end.end(), s.end.begin(), s.end.end());
	if (s.skip)
		skip = true;
	return *this;
}

generic_t::generic_t()
{
	peg_t::load(peg);
}

generic_t::~generic_t()
{
}

segment_t generic_t::load_term(lexer_t &lexer, const token_t &token, grammar_t &grammar)
{
	bool keep = true;
	segment_t result;
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
		symbol_t *term = NULL;
		if (word == "(") {
			i++;
			if (i->type == CHOICE) {
				result = load_choice(lexer, *i, grammar);
			}
		} else if (i->type == TEXT) {
			term = grammar.insert(new keyword(word.substr(1, word.size()-2), keep));
		} else if (i->type == CHARACTER_CLASS) {
			term = grammar.insert(new character(word.substr(1, word.size()-2), keep));
		} else if (i->type == NAME) {
			if (word == "instance")
				term = grammar.insert(new instance(keep));
			else if (word == "text")
				term = grammar.insert(new text(keep));
			else if (word == "_")
				term = grammar.insert(new whitespace(true));
			else if (word == "__")
				term = grammar.insert(new whitespace(false));
			else if (word.compare(0, 7, "integer") == 0) {
				std::string base = word.substr(7);
				if (base.size() > 0)
					term = grammar.insert(new integer(atoi(base.c_str()), keep));
				else
					term = grammar.insert(new integer(10, keep));
			}
			else if (word == "character_class")
				term = grammar.insert(new character_class(keep));
			else
			{
				size_t space = word.rfind("::");
				if (space == std::string::npos)
					word = lexer.basename + "::" + word;

				std::map<std::string, int>::iterator definition = definitions.find(word);
				if (definition != definitions.end())
					term = grammar.insert(new stem(definition->second, keep));
				else
				{
					int index = grammar.rules.size();
					grammar.rules.push_back(rule_t(grammar.rules.size(), word));
					definitions.insert(std::pair<std::string, int>(word, index));
					term = grammar.insert(new stem(index, keep));
				}
			}
		} else {
			result.msgs.push_back(fail(lexer, *i) << "unrecognied term type '" << i->type << "'.");
		}

		if (term != NULL) {
			result.start.push_back(term);
			result.end.push_back(term);
		}
	}

	if (token.tokens.size() > 0)
	{
		std::string attr = lexer.read(token.tokens.back().begin, token.tokens.back().end);
		if (attr == "*" or attr == "+") {
			std::vector<symbol_t*>::iterator i, j;
			for (std::vector<symbol_t*>::iterator i = result.end.begin(); i != result.end.end(); i++)
				for (std::vector<symbol_t*>::iterator j = result.start.begin(); j != result.start.end(); j++)
					(*i)->next.push_back(*j);
		}

		if (attr == "?" or attr == "*")
			result.skip = true;
	}

	return result;
}

segment_t generic_t::load_sequence(lexer_t &lexer, const token_t &token, grammar_t &grammar)
{
	segment_t result;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == TERM)
			result.sequence(load_term(lexer, *i, grammar));
		else if (i->type == SEQUENCE)
			result.sequence(load_sequence(lexer, *i, grammar));
	}

	return result;
}

segment_t generic_t::load_choice(lexer_t &lexer, const token_t &token, grammar_t &grammar)
{
	segment_t result;
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == TERM)
			result.parallel(load_term(lexer, *i, grammar));
		else if (i->type == SEQUENCE)
			result.parallel(load_sequence(lexer, *i, grammar));
		else if (i->type == CHOICE)
			result.parallel(load_choice(lexer, *i, grammar));
	}

	return result;
}

void generic_t::load_definition(lexer_t &lexer, const token_t &token, grammar_t &grammar)
{
	std::vector<token_t>::const_iterator curr = token.tokens.begin();

	std::string name;

	if (curr->type == INSTANCE) {
		name = lexer.basename + "::" + lexer.read(curr->begin, curr->end);
		curr++;
	} else {
		std::cout << (fail(lexer, token) << "incorrect format for 'definition' should have been caught by the parser");
	}

	bool atomic = true;
	bool keep = true;
	if (curr->type == KEYWORD && lexer.read(curr->begin, curr->end) == "~") {
		keep = false;
		curr++;
	}

	if (curr->type == KEYWORD && lexer.read(curr->begin, curr->end) == "@") {
		atomic = false;
		curr++;
	}

	std::map<std::string, int>::iterator result = definitions.lower_bound(name);
	if (result == definitions.end() || result->first != name) {
		result = definitions.insert(result, std::pair<std::string, int>(name, (int)grammar.rules.size()));
		grammar.rules.push_back(rule_t(grammar.rules.size(), name, keep, atomic));
	} else {
		grammar.rules[result->second].atomic = atomic;
		grammar.rules[result->second].keep = keep;
	}
	
	if (grammar.rules[result->second].start.size() == 0) {
		if (curr->type == CHOICE) {
			segment_t seg = load_choice(lexer, *curr, grammar);
			for (int i = 0; i < (int)seg.msgs.size(); i++)
				std::cout << seg.msgs[i];

			for (int i = 0; i < (int)seg.start.size(); i++)
				grammar.rules[result->second].start.push_back(seg.start[i]);
			if (seg.skip)
				grammar.rules[result->second].start.push_back(NULL);

			for (std::vector<symbol_t*>::iterator i = seg.end.begin(); i != seg.end.end(); i++)
				(*i)->next.push_back(NULL);
		} else {
			std::cout << (fail(lexer, token) << "incorrect format for 'definition' should have been caught by the parser");
		}
	} else {
		std::cout << (error(lexer, token) << "multiple definitions for '" << name << "'.");
	}
}

void generic_t::load_import(lexer_t &lexer, const token_t &token, grammar_t &grammar)
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

				parsing result = peg.parse(sublexer);
				if (result.msgs.size() == 0) {
					load_grammar(sublexer, result.tree, grammar);
				} else {
					std::cout << (note(lexer, token) << "imported from '" << name << "':");
					for (int i = 0; i < (int)result.msgs.size(); i++)
						std::cout << result.msgs[i];
				}
			}

			sublexer.close();
		}
		else
			std::cout << (error() << "file '" << name << "' not found.");
	} else {
		std::cout << (fail(lexer, token) << "incorrect format for 'import' should have been caught by the parser");
	}
}

void generic_t::load_grammar(lexer_t &lexer, const token_t &token, grammar_t &grammar)
{
	for (std::vector<token_t>::const_iterator i = token.tokens.begin(); i != token.tokens.end(); i++)
	{
		if (i->type == DEFINITION)
			load_definition(lexer, *i, grammar);
		else if (i->type == IMPORT)
			load_import(lexer, *i, grammar);
		else if (i->type == PEG)
			load_grammar(lexer, *i, grammar);
		else if (i->type != CHARACTER)
			std::cout << (fail(lexer, token) << "unrecognized grammar type '" << i->type << "'." << DEFINITION);
	}
}

void generic_t::load(std::string filename, grammar_t &grammar)
{
	lexer_t lexer;
	if (lexer.open(filename))
	{
		std::vector<std::string>::iterator loc = std::lower_bound(imports.begin(), imports.end(), lexer.basename);
		if (loc == imports.end() || *loc != lexer.basename) {
			loc = imports.insert(loc, lexer.basename);

			parsing result = peg.parse(lexer);
			if (result.msgs.size() == 0) {
				load_grammar(lexer, result.tree, grammar);

				for (int i = 0; i < (int)grammar.rules.size(); i++)
					if (grammar.rules[i].start.size() == 0)
						std::cout << (warning() << "definition '" << grammar.rules[i].name << "' not found.");
			} else
				for (int i = 0; i < (int)result.msgs.size(); i++)
					std::cout << result.msgs[i];
		}

		lexer.close();
	}
	else
		std::cout << (error() << "file '" << filename << "' not found.");
}

std::string toConst(std::string str)
{
	size_t start = str.find_last_of(":");
	for (size_t i = start; i < str.size(); i++)
		str[i] = ::toupper(str[i]);
	return str.substr(start+1);
}

void export_grammar(const grammar_t &grammar, std::string space, std::string name, std::ostream &header, std::ostream &source)
{
	using std::endl;
	
	header << "#pragma once" << endl << endl;
	header << "#include <pgen/grammar.h>" << endl << endl;
	if (space.size() > 0) {
		header << "namespace " << space << endl;
		header << "{" << endl << endl;
	}
	header << "struct " << name << "_t" << endl;
	header << "{" << endl;
	for (int i = 0; i < (int)grammar.rules.size(); i++)
		header << "\tint32_t " << toConst(grammar.rules[i].name) << ";" << endl;
	header << endl;
	header << "\tvoid load(grammar_t &grammar);" << endl;
	header << "};" << endl << endl;
	if (space.size() > 0) {
		header << "}" << endl << endl;
	}	

	source << "#include \"" << name << ".h\"" << endl << endl;
	source << "#include <pgen/default.h>" << endl << endl;

	if (space.size() > 0) {
		source << "namespace " << space << endl;
		source << "{" << endl << endl;
	}

	source << "void " << name << "_t::load(grammar_t &grammar)" << endl;
	source << "{" << endl;

	for (int i = 0; i < (int)grammar.rules.size(); i++) {
		source << "\t" << toConst(grammar.rules[i].name) << " = grammar.rules.size();" << endl;
		source << "\tgrammar.rules.push_back(rule_t(" << toConst(grammar.rules[i].name) << ", \"" << grammar.rules[i].name << "\", " << (grammar.rules[i].keep ? "true" : "false") << ", " << (grammar.rules[i].atomic ? "true" : "false") << "));" << endl;
	}
	source << endl;

	int size = 0;
	for (symbol_t *i = grammar.symbols; i != NULL; i = i->right)
		size++;

	source << "\tsymbol_t *n[" << size << "];" << endl; 
	int index = 0;
	std::map<const symbol_t*, int> indices;
	for (symbol_t *i = grammar.symbols; i != NULL; i = i->right)
	{
		indices.insert(std::pair<const symbol_t*, int>(i, index));
		source << "\tn[" << index++ << "] = grammar.insert(new " << i->emit() << ");" << endl;
	}
	source << endl;

	index = 0;
	for (symbol_t *i = grammar.symbols; i != NULL; i = i->right)
	{
		for (std::vector<symbol_t*>::iterator j = i->next.begin(); j != i->next.end(); j++)
		{
			if (*j == NULL)
				source << "\tn[" << index << "]->next.push_back(NULL);" << endl;
			else
			{
				std::map<const symbol_t*, int>::iterator nextindex = indices.find(*j);
				if (nextindex != indices.end())
					source << "\tn[" << index << "]->next.push_back(n[" << nextindex->second << "]);" << endl;
				else
					printf("link not found from %d\n", index);
			}
		}
		++index;
	}
	source << endl;

	for (int i = 0; i < (int)grammar.rules.size(); i++)
	{
		for (std::vector<symbol_t*>::const_iterator j = grammar.rules[i].start.begin(); j != grammar.rules[i].start.end(); j++)
		{
			if (*j == NULL)
				source << "\tgrammar.rules[" << i << "].start.push_back(NULL);" << endl;
			else
			{
				std::map<const symbol_t*, int>::iterator nextindex = indices.find(*j);
				if (nextindex != indices.end())
					source << "\tgrammar.rules[" << i << "].start.push_back(n[" << nextindex->second << "]);" << endl;
				else
					printf("link not found from rule %d\n", i);
			}
		}
	}

	source << "}" << endl << endl;

	if (space.size() > 0) {
		source << "}" << endl << endl;
	}
}


}
