#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace pgen
{

struct lexer_t
{
	lexer_t();
	~lexer_t();

	FILE *ptr;
	std::string text;

	std::string name;
	std::string basename;
	std::vector<intptr_t> lines;
	intptr_t offset;
	intptr_t line;
	
	operator bool();

	bool open(std::string filename);
	void close();

	int lineof(intptr_t pos) const;

	intptr_t cache(intptr_t pos);
	
	void moveto(intptr_t pos);

	std::string getline(intptr_t line);
	std::string read(intptr_t begin, intptr_t end);

	bool eof();
	char get();
	void unget();
};

}
