#include "lex.h"

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

lex::lex()
{
	ptr = NULL;
	offset = -1;
	line = -1;
	prev = '\0';
	curr = '\n';
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
	offset = -1;
	line = -1;
	prev = '\0';
	curr = '\n';
	lines.clear();
}

int lex::lineof(intptr_t pos) const
{
	return *upper_bound(lines.begin(), lines.end(), pos);
}

void lex::moveto(intptr_t pos)
{
	if (offset <= pos && lines.back() <= pos)
	{
		if (offset < lines.back())
		{
			fseek(ptr, lines.back()-1, SEEK_SET);
			prev = '\n';
			curr = fgetc(ptr);
			offset = lines.back();
			line = lines.size()-1;
		}

		while (not feof(ptr) and offset < pos)
			get();
	}
	else if (pos > 0)
	{
		fseek(ptr, pos-1, SEEK_SET);
		prev = fgetc(ptr);
		curr = fgetc(ptr);
		offset = pos;
		line = lineof(pos);
	}
	else if (pos >= 0)
	{
		fseek(ptr, pos, SEEK_SET);
		prev = '\n';
		curr = fgetc(ptr);
		offset = pos;
		line = lineof(pos);
	}
	else
	{
		fseek(ptr, 0, SEEK_SET);
		prev = '\0';
		curr = '\n';
		offset = -1;
		line = -1;
	}
}

string lex::getline(intptr_t line)
{
	return read(lines[line], lines[line+1]);
}

string lex::read(intptr_t begin, intptr_t end)
{
	string result(end-begin+1, '\0');
	fseek(ptr, begin, SEEK_SET);
	fread(&result[0], sizeof(char), result.size()-1, ptr);
	fseek(ptr, offset+1, SEEK_SET);
	return result;
}

bool lex::eof()
{
	return feof(ptr);
}

char lex::get()
{
	prev = curr;
	curr = fgetc(ptr);
	++offset;

	if (prev == '\n')
	{
		++line;
		if (line >= (intptr_t)lines.size())
			lines.push_back(offset);
	}

	return curr;
}

