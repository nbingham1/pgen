#include <parse/grammar.h>
#include <map>

namespace parse
{

parsing::parsing(intptr_t begin, intptr_t end) : tree(begin, end)
{
	stem = -1;
}

parsing::~parsing()
{
}

void parsing::emit(lexer_t &lexer)
{
	tree.emit(lexer);
	for (int i = 0; i < (int)msgs.size(); i++)
		std::cout << msgs[i];
}

/********************** symbol_t **********************/

symbol_t::symbol_t(bool keep)
{
	this->keep = keep;
}

symbol_t::~symbol_t()
{
}

/********************** rule_t **********************/

rule_t::rule_t()
{
}

rule_t::rule_t(int32_t type, std::string name, bool keep, bool atomic)
{
	this->type = type;
	this->name = name;
	this->atomic = atomic;
	this->keep = keep;
}

rule_t::~rule_t()
{
}

/********************** branch_t **********************/

branch_t::branch_t()
{
}

branch_t::branch_t(const rule_t &rule, int parent, intptr_t offset, bool keep)
{
	this->branches = rule.start;

	this->name = rule.name;
	this->tree.type = rule.type;
	this->tree.begin = offset;
	this->tree.end = offset;

	this->parent = parent;
	this->branch = 0;
	this->curr = branches[branch];

	this->keep = keep;
	this->atomic = rule.atomic;
}

branch_t::branch_t(symbol_t *point, int parent, intptr_t offset) 
{
	this->parent = parent;
	this->tree.begin = offset;
	this->tree.end = offset;
	if (point)
	{
		this->branches = point->next;
		this->branch = 0;
		this->curr = branches[branch];
	}
	else
	{
		this->branch = 0;
		this->curr = point;
	}
	this->keep = true;
	this->atomic = false;
}

branch_t::~branch_t()
{
}

/********************** branches_t **********************/

branches_t::branches_t()
{
	frame = -1;
}

branches_t::branches_t(const rule_t &rule, intptr_t offset)
{
	elems.push_back(branch_t(rule, -1, offset, true));
	frame = 0;
}

branches_t::~branches_t()
{
}

branches_t::operator bool()
{
	return (int)elems.size() > 0 && (elems.back().curr || frame >= 0);
}

void branches_t::push(symbol_t *point, intptr_t offset)
{
	elems.push_back(branch_t(point, frame, offset));
}

void branches_t::pop()
{
	elems.pop_back();
}

void branches_t::push_frame(const rule_t &rule, intptr_t offset, bool keep)
{
	elems.push_back(branch_t(rule, frame, offset, keep));
	frame = (int)elems.size()-1;
}

void branches_t::pop_frame(intptr_t offset)
{
	if (frame >= 0 && elems[frame].atomic) {
		collapse_frame(frame);
	}

	if (frame > 0)
		elems.push_back(branch_t(elems[frame-1].curr, elems[frame].parent, offset));
	else
		elems.push_back(branch_t(NULL, elems[frame].parent, offset));
	frame = elems.back().parent;
}

bool branches_t::rewind(lexer_t &lexer, std::vector<message> *msgs)
{
	if (elems.back().branch < (int)elems.back().branches.size())
	{
		++(elems.back().branch);
		while ((int)elems.size() > 1 and elems.back().branch >= (int)elems.back().branches.size())
		{
			if (msgs != NULL and elems.back().name.size() > 0)
				msgs->push_back(error(lexer, elems.back().tree) << "expected " << elems.back().name << ".");
			elems.pop_back();
			++(elems.back().branch);
		}
	}

	if (elems.back().branch >= (int)elems.back().branches.size())
	{
		elems.pop_back();
		return false;
	}
	else
	{
		lexer.moveto(elems.back().tree.begin);
		elems.back().tree.clear();
		elems.back().curr = elems.back().branches[elems.back().branch];
		if (elems.back().name.size() > 0)
			frame = (int)elems.size()-1;
		else
			frame = elems.back().parent;
		return true;
	}
}

void branches_t::advance()
{
	if ((int)elems.back().curr->next.size() == 0)
		elems.back().curr = NULL;
	else if ((int)elems.back().curr->next.size() == 1)
		elems.back().curr = elems.back().curr->next[0];
	else {
		elems.push_back(branch_t(elems.back().curr, frame, elems.back().tree.end));
	}
}

branch_t &branches_t::back()
{
	return elems.back();
}

const symbol_t *branches_t::curr()
{
	return elems.back().curr;
}

void branches_t::collapse_frame(int index)
{
	int start = index;
	elems[start].tree = collect_frame(index);
	elems.erase(elems.begin()+start+1, elems.begin()+index); 
}


token_t branches_t::collect_frame(int &index)
{
	int start = index;
	token_t result;
	while (index < (int)elems.size())
	{
		if (index == start)
			result = elems[index++].tree;
		else if (elems[index].parent == start)
		{
			if (elems[index].name.size() > 0)
			{
				bool keep = elems[index].keep;
				token_t temp = collect_frame(index);
				if (keep)
					result.append(temp);
			}
			else
				result.extend(elems[index++].tree);
		}
		else
			return result;
	}
	return result;
}

token_t branches_t::collect()
{
	int index = 0;
	return collect_frame(index);
}

/********************** grammar_t **********************/

grammar_t::grammar_t()
{
	symbols = NULL;
}

grammar_t::~grammar_t()
{
	symbol_t *curr = symbols, *prev;
	while (curr != NULL)
	{
		prev = curr;
		curr = curr->right;
		delete prev;
	}
	symbols = NULL;
}

symbol_t *grammar_t::insert(symbol_t *sym)
{
	sym->right = symbols;
	symbols = sym;
	return sym;
}

parsing grammar_t::parse(lexer_t &lexer, int index)
{
	parsing best;
	branches_t stack(rules[index], lexer.offset);

	while (stack)
	{
		if (stack.curr() == NULL)
			stack.pop_frame(lexer.offset);
		else
		{
			parsing result = stack.curr()->parse(lexer);

			if (result.stem >= 0)
				stack.push_frame(rules[result.stem], lexer.offset, rules[result.stem].keep and stack.curr()->keep);
			else
			{
				if (stack.curr()->keep)
					stack.back().tree.append(result.tree);
				else
					stack.back().tree.skip(result.tree);

				if ((int)result.msgs.size() > 0)
				{
					if (result.tree.end >= best.tree.end)
					{
						best.tree = stack.collect();
						best.msgs = result.msgs;
						stack.rewind(lexer, &best.msgs);
					}
					else
						stack.rewind(lexer);
				}
				else
					stack.advance();
			}
		}
	}

	if ((int)stack.elems.size() == 0)
		return best;
	else
	{
		parsing result;
		result.tree = stack.collect();
		return result;
	}
}

}
