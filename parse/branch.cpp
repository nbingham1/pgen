#include <parse/branch.h>

namespace parse
{

/********************** branch_t **********************/

branch_t::branch_t()
{
}

branch_t::branch_t(const grammar_t::rule &rule, int parent, intptr_t offset, bool keep)
{
	this->branches = rule.start;

	this->tree.type = rule.name;
	this->tree.begin = offset;
	this->tree.end = offset;

	this->parent = parent;
	this->branch = 0;
	this->curr = branches[branch];

	this->keep = keep;
	this->atomic = rule.atomic;
}

branch_t::branch_t(grammar_t::const_iterator point, int parent, intptr_t offset)
{
	this->parent = parent;
	this->tree.begin = offset;
	this->tree.end = offset;
	if (point)
	{
		this->branches = point.next();
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

branches_t::branches_t(const grammar_t::rule &rule, intptr_t offset)
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

void branches_t::push(grammar_t::const_iterator point, intptr_t offset)
{
	elems.push_back(branch_t(point, frame, offset));
}

void branches_t::pop()
{
	elems.pop_back();
}

void branches_t::push_frame(const grammar_t::rule &rule, intptr_t offset, bool keep)
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
		elems.push_back(branch_t(grammar_t::const_iterator(), elems[frame].parent, offset));
	frame = elems.back().parent;
}

bool branches_t::rewind(lexer_t &lexer, std::vector<message> *msgs)
{
	//printf("Rewind: %d, %d/%d\n", (int)elems.size(), elems.back().branch, (int)elems.back().branches.size());

	if (elems.back().branch < (int)elems.back().branches.size())
	{
		++(elems.back().branch);
		while ((int)elems.size() > 1 and elems.back().branch >= (int)elems.back().branches.size())
		{
			if (msgs != NULL and elems.back().tree.type.size() > 0)
			{
				msgs->push_back(message(message::error, "expected " + elems.back().tree.type, lexer, true, elems.back().tree.begin, elems.back().tree.end));
			}
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
		//elems.back().tree.emit(lexer);
		lexer.moveto(elems.back().tree.begin);
		elems.back().tree.clear();
		elems.back().curr = elems.back().branches[elems.back().branch];
		if (elems.back().tree.type.size() > 0)
			frame = (int)elems.size()-1;
		else
			frame = elems.back().parent;
		return true;
	}
}

void branches_t::advance()
{
	if ((int)elems.back().curr.next().size() == 0)
		elems.back().curr.loc = NULL;
	else if ((int)elems.back().curr.next().size() == 1)
		elems.back().curr = elems.back().curr.next()[0];
	else {
		elems.push_back(branch_t(elems.back().curr, frame, elems.back().tree.end));
	}
}

branch_t &branches_t::back()
{
	return elems.back();
}

grammar_t::const_iterator &branches_t::curr()
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
			if (elems[index].tree.type.size() > 0)
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

}
