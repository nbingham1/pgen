#include <parse/lexer.h>

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

namespace parse
{

lexer_t::lexer_t()
{
	ptr = NULL;
	offset = 0;
	line = 0;
	lines.push_back(0);
}

lexer_t::~lexer_t()
{
}

lexer_t::operator bool()
{
	return ptr != NULL;
}

bool lexer_t::open(std::string filename)
{
	if (ptr != NULL)
		return false;
	
	name = filename;
	ptr = fopen(name.c_str(), "r");
	return ptr != NULL;
}

void lexer_t::close()
{
	fclose(ptr);
	ptr = NULL;
	offset = 0;
	line = 0;
	lines.clear();
	lines.push_back(0);
}

int lexer_t::lineof(intptr_t pos) const
{
	std::vector<intptr_t>::const_iterator i = upper_bound(lines.begin(), lines.end(), pos)-1;
	return i-lines.begin();
}

void lexer_t::moveto(intptr_t pos)
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

std::string lexer_t::getline(intptr_t target)
{
	if (target+1 >= (int)lines.size())
	{
		std::string result;

		intptr_t prev_offset = offset;
		intptr_t prev_line = line;
		fseek(ptr, lines.back(), SEEK_SET);
		offset = lines.back();
		line = lines.size()-1;

		while (not feof(ptr) and target+1 >= (int)lines.size())
		{
			char c = get();
			if (line == target)
				result.push_back(c);
		}

		offset = prev_offset;
		line = prev_line;
		fseek(ptr, offset, SEEK_SET);
		return result;
	}
	else
		return read(lines[target], lines[target+1]);
}

std::string lexer_t::read(intptr_t begin, intptr_t end)
{
	std::string result(end-begin+1, '\0');
	fseek(ptr, begin, SEEK_SET);
	size_t total = fread(&result[0], sizeof(char), result.size()-1, ptr);
	result.resize(total);
	fseek(ptr, offset, SEEK_SET);
	return result;
}

bool lexer_t::eof()
{
	return feof(ptr);
}

char lexer_t::get()
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

void lexer_t::unget()
{
	fseek(ptr, -1, SEEK_CUR);
	--offset;
	if (offset < lines[line])
		--line;
}

}
