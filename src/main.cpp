#include <fstream>

#include <parse/lexer.h>
#include <parse/generic.h>

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		parse::generic_t peg;
		parse::grammar_t gram;
		peg.load(argv[i], gram);

		std::string path = argv[i];

		size_t slash = std::string::npos, dot = std::string::npos;
		slash = path.rfind("/");
		dot = path.rfind(".");
		slash = slash == std::string::npos ? 0 : slash+1;
		dot = dot == std::string::npos ? path.size() : dot;

		std::string name = path.substr(slash, dot-slash);

		std::ofstream header(path.substr(0, dot) + ".h");
		std::ofstream source(path.substr(0, dot) + ".cpp");
		export_grammar(gram, "parse", name, header, source);
	}
}

