#include <pgen/lexer.h>

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

namespace pgen {

lexer_t::lexer_t() {
	ptr = NULL;
	offset = 0;
	line = 0;
	lines.push_back(0);
}

lexer_t::~lexer_t() {
	close();
}

lexer_t::operator bool() {
	return ptr != NULL;
}

bool lexer_t::open(std::string filename) {
	if (ptr != NULL) {
		return false;
	}
	
	name = filename;
	basename = filename;

	size_t slash = basename.find_last_of('/');
	if (slash != std::string::npos) {
		basename = basename.substr(slash+1);
	}

	size_t dot = basename.find_last_of('.');
	if (dot != std::string::npos) {
		basename = basename.substr(0, dot);
	}

	ptr = fopen(name.c_str(), "r");
	fseek(ptr, 0, SEEK_END);
	text.reserve(ftell(ptr));
	fseek(ptr, 0, SEEK_SET);
	return ptr != NULL;
}

void lexer_t::close() {
	if (ptr != NULL) {
		fclose(ptr);
		ptr = NULL;
	}
	offset = 0;
	line = 0;
	lines.clear();
	lines.push_back(0);
}

int lexer_t::lineof(intptr_t pos) const {
	std::vector<intptr_t>::const_iterator i = upper_bound(lines.begin(), lines.end(), pos)-1;
	return i-lines.begin();
}

intptr_t lexer_t::cache(intptr_t pos) {
	if (pos < 0) {
		pos = 0;
	}

	if (pos >= (intptr_t)text.size()) {
		intptr_t prev_offset = offset;
		intptr_t prev_line = line;
		offset = text.size();
		line = lines.size()-1;

		while (not eof() and pos >= (intptr_t)text.size()) {
			get();
		}

		offset = prev_offset;
		line = prev_line;
	}

	if (pos >= (intptr_t)text.size()) {
		pos = (intptr_t)text.size();
	}

	return pos;
}

void lexer_t::moveto(intptr_t pos) {
	offset = cache(pos);
	line = lineof(offset);
}

std::string lexer_t::getline(intptr_t target) {
	if (target+1 >= (int)lines.size()) {
		intptr_t prev_offset = offset;
		intptr_t prev_line = line;
		offset = text.size();
		line = lines.size()-1;

		while (not eof() and target+1 >= (intptr_t)lines.size()) {
			get();
		}

		offset = prev_offset;
		line = prev_line;
	}
	return text.substr(lines[target], lines[target+1]-lines[target]);
}

std::string lexer_t::read(intptr_t begin, intptr_t end) {
	return text.substr(begin, cache(end)-begin);
}

bool lexer_t::eof() {
	return offset >= (intptr_t)text.size() and feof(ptr);
}

char lexer_t::get() {
	const intptr_t PAGE = 4096;

	if (offset >= (intptr_t)text.size()) {
		intptr_t prev = text.size();
		text.resize(prev+PAGE);
		size_t total = fread(&text[prev], sizeof(char), PAGE, ptr);
		text.resize(prev+total);
	}

	if (offset >= (intptr_t)text.size()) {
		if (lines.back() != offset) {
			lines.push_back(offset);
		}
		return '\0';
	}

	int curr = text[offset];
	++offset;
	if (curr == '\n') {
		++line;
		if (line >= (intptr_t)lines.size()) {
			lines.push_back(offset);
		}
	}
	
	return curr;
}

void lexer_t::unget() {
	--offset;
	if (offset < lines[line])
		--line;
}

}
