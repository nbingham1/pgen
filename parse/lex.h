#pragma once

#include <vector>
#include <string>

using namespace std;

struct lex
{
	lex();
	~lex();

	FILE *ptr;
	string name;
	vector<intptr_t> lines;
	intptr_t offset;
	intptr_t line;
	
	operator bool();

	bool open(string filename);
	void close();

	int lineof(intptr_t pos) const;
	
	void moveto(intptr_t pos);

	string getline(intptr_t line);
	string read(intptr_t begin, intptr_t end);

	bool eof();
	char get();
};

