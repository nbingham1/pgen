#include "lex.h"

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

lex::lex()
{
	ptr = NULL;
	offset = 0;
	line = 0;
	lines.push_back(0);
}

lex::~lex()
{
}

lex::operator bool()
{
	return ptr != NULL;
}

bool lex::open(string filename)
{
	if (ptr != NULL)
		return false;
	
	name = filename;
	ptr = fopen(name.c_str(), "r");
	return ptr != NULL;
}

void lex::close()
{
	fclose(ptr);
	ptr = NULL;
	offset = 0;
	line = 0;
	lines.clear();
	lines.push_back(0);
}

int lex::lineof(intptr_t pos) const
{
	vector<intptr_t>::const_iterator i = upper_bound(lines.begin(), lines.end(), pos)-1;
	return i-lines.begin();
}

void lex::moveto(intptr_t pos)
{
	if (pos < 0)
		pos = 0;

	if (offset <= pos && lines.back() <= pos)
	{
		if (offset < lines.back())
		{
			fseek(ptr, lines.back(), SEEK_SET);
			offset = lines.back();
			line = lines.size()-1;
		}

		while (not feof(ptr) and offset < pos)
			get();
	}
	else
	{
		fseek(ptr, pos, SEEK_SET);
		offset = pos;
		line = lineof(pos);
	}
}

string lex::getline(intptr_t line)
{
	if (line+1 >= (int)lines.size())
		return string();
	else
		return read(lines[line], lines[line+1]);
}

string lex::read(intptr_t begin, intptr_t end)
{
	string result(end-begin+1, '\0');
	fseek(ptr, begin, SEEK_SET);
	fread(&result[0], sizeof(char), result.size()-1, ptr);
	fseek(ptr, offset, SEEK_SET);
	return result;
}

bool lex::eof()
{
	return feof(ptr);
}

char lex::get()
{
	int curr = fgetc(ptr);
	if (curr != EOF)
	{
		++offset;

		if (curr == '\n')
		{
			++line;
			if (line >= (intptr_t)lines.size())
				lines.push_back(offset);
		}
	
		return curr;
	}
	else if (lines.back() != offset)
	{
		++offset;
		lines.push_back(offset);
	}
	
	return 0;
}

