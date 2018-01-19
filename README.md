# pgen

pgen is an extremely flexible parser generator for C++ with multiple interface levels.

## Basics

People new to pgen may generate parsers from text files using the `pgen` executable.
```bash
./pgen file1 file2 ... filen
```

Source files must be written in the following Parsing Expression Grammar format.
```peg
peg = definition* _ [\0];
definition = instance _ "=" _ choice _ ";";
choice = sequence (_ "|" _ sequence)*;
sequence = term (_ term)*;
term = ("(" _ choice _ ")" | text | instance | character_class) (_ ("?" | "*" | "+"))?;
```

This interface sacrifices some flexibility in favor of speed by using the following basic hard-coded
symbols defined in [parse/default.h](parse/default.h):
```peg
instance = [a-zA-Z_][a-zA-Z0-9_]*;
text = "\"" [^\"]* "\"";
_ = [ \t\n\r]*;
integer = [0-9]+;
character_class = "[" [^\]]+ "]";
```
This will generate a header and source file with your grammar. You can use these headers the same way the
PEG parser in [parse/peg.h](parse/peg.h) is used.

```cpp
// Initialize the grammar
parse::peg_t peg;

// Load the file into the lexer
parse::lexer_t lexer;
lexer.open(filename);

// Parse the file with the grammar
parse::parsing result = peg.parse(lexer);
if (result.msgs.size() == 0)
{
  // no errors, print the parsed abstract syntax tree
  result.tree.emit(lexer);
}
else
{
  // there were parsing errors, print them out
  for (int i = 0; i < (int)result.msgs.size(); i++)
    result.msgs[i].emit();
}
```

The resulting syntax tree is a hierarchy of tokens as defined in [parse/token.h](parse/token.h).

* `std::string type;` is the name of the rule that this token came from.
* `intptr_t begin, end;` is the byte offset into the file of the beginning and end of the token.
* `std::vector<token_t> tokens;` is an array of tokens that were found while parsing this rule.

To get the source text of a token, you will need the lexer.
```
lexer.read(token.begin, token.end);
```

If you parsed a grammar with the PEG parser, the abstract syntax tree can be loaded into a grammar
with [parse/generic.h](parse/generic.h).
```
parse::generic_t gen;
gen.load(lexer, result.tree);
```
Once loaded, the
grammar can be used or saved into a C++ class file for later.
```
std::ofstream header(headername);
std::ofstream source(sourcename);
gen.save("parse", name, header, source);
```
## Under the Hood

A grammar is represented as a graph of symbols. New hard-coded symbols may be added by inheriting
from `grammar_t::symbol` in [parse/grammar.h](parse/grammar.h). For an example, see
[parse/default.h](parse/default.h). Each symbol must implement three functions:

```
parsing parse(lexer_t &lexer) const;
grammar_t::symbol *clone(int rule_offset = 0) const;
std::string emit() const;
```

The `parse()` function is the primary workhorse, returning a `parsing` structure as defined in
[parse/grammar.h](parse/grammar.h). A parsing has three members.

* `int stem;` is the index of another definition you want to descend into.
* `token_t tree;` is the resulting AST of this symbol.
* `std::vector<message> msgs;` is an array of error messages produced during the parsing.

The `clone()` function is used when you want to import one grammar into another, and the `emit()`
function is used when you want to save the grammar to a C++ class.
