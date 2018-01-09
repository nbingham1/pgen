#pragma once

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
	int line;
	char prev, curr;
	
	operator bool();

	bool open(string filename);
	void close();

	void moveto(intptr_t offset);

	char get();
};

