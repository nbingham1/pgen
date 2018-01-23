#include <fstream>

#include <parse/peg.h>
#include <parse/lexer.h>
#include <parse/generic.h>

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		parse::generic_t gen;
		gen.load(argv[i]);
		std::string name = argv[i];
		if (name.substr(name.size()-4) == ".peg")
			name = name.substr(0, name.size()-4);

		std::ofstream header(name + ".h");
		std::ofstream source(name + ".cpp");
		gen.save("parse", name, header, source);
	}
}

