#include <fstream>

#include <parse/peg.h>
#include <parse/lexer.h>
#include <parse/generic.h>

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		parse::lexer_t lexer;
		lexer.open(argv[i]);
		parse::peg_t peg;

		parse::parsing result = parse::parse(peg, lexer, peg.load_peg());
		if (result.msgs.size() == 0)
		{
			parse::generic_t gen;
			gen.load(lexer, result.tree);
			std::string name = argv[i];
			if (name.substr(name.size()-5) == ".gram")
				name = name.substr(0, name.size()-5);

			std::ofstream header(name + ".h");
			std::ofstream source(name + ".cpp");
			gen.save("parse", name, header, source);
		}
		else
			for (int i = 0; i < (int)result.msgs.size(); i++)
				result.msgs[i].emit();
	}
}

