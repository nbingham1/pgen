#include "lex.h"

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

void lex::moveto(intptr_t pos)
{
	if (lines.back() <= pos)
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
	else
	{
		fseek(ptr, pos-2, SEEK_SET);
		prev = fgetc(ptr);
		curr = fgetc(ptr);
		offset = pos;
		line = *upper_bound(lines.begin(), lines.end(), pos);
	}
}

char lex::get()
{
	prev = curr;
	curr = fgetc(ptr);
	++offset;

	if (prev == '\n')
	{
		++line;
		if (line >= lines.size())
			lines.push_back(offset);
	}

	return curr;
}

