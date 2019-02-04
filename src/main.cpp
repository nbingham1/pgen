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
		std::string name = argv[i];
		if (name.substr(name.size()-4) == ".peg")
			name = name.substr(0, name.size()-4);

		std::ofstream header(name + ".h");
		std::ofstream source(name + ".cpp");
		export_grammar(gram, "parse", name, header, source);
	}
}

