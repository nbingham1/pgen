#include <parse/grammar.h>
#include <parse/branch.h>
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
		msgs[i].emit();
}

/********************** grammar_t::node **********************/

grammar_t::node::node()
{
	left = this;
	right = this;
}

grammar_t::node::~node()
{
}

/********************** grammar_t::symbol **********************/

grammar_t::symbol::symbol(bool keep)
{
	this->keep = keep;
}

grammar_t::symbol::~symbol()
{
}

/********************** grammar_t::iterator **********************/

grammar_t::iterator::iterator()
{
	loc = NULL;
}

grammar_t::iterator::iterator(node *loc)
{
	this->loc = loc;
}

grammar_t::iterator::iterator(const iterator &copy)
{
	loc = copy.loc;
}

grammar_t::iterator::operator bool() const
{
	return loc != NULL and loc->left != loc and loc->right != loc;
}

grammar_t::symbol &grammar_t::iterator::operator*() const
{
	return *(grammar_t::symbol*)loc;
}

grammar_t::symbol *grammar_t::iterator::operator->() const
{
	return (grammar_t::symbol*)loc;
}

grammar_t::iterator grammar_t::iterator::operator++(int)
{
	iterator result(loc);
	loc = loc->right;
	return result;
}

grammar_t::iterator grammar_t::iterator::operator--(int)
{
	iterator result(loc);
	loc = loc->left;
	return result;
}

grammar_t::iterator &grammar_t::iterator::operator++()
{
	loc = loc->right;
	return *this;
}

grammar_t::iterator &grammar_t::iterator::operator--()
{
	loc = loc->left;
	return *this;
}

grammar_t::iterator &grammar_t::iterator::operator+=(int n)
{
	for (int i = 0; i < n && loc->right != loc; i++)
		loc = loc->right;

	return *this;
}

grammar_t::iterator &grammar_t::iterator::operator-=(int n)
{
	for (int i = 0; i < n && loc->left != loc; i++)
		loc = loc->left;

	return *this;
}

grammar_t::iterator grammar_t::iterator::operator+(int n) const
{
	iterator result(loc);
	result += n;
	return result;
}

grammar_t::iterator grammar_t::iterator::operator-(int n) const
{
	iterator result(loc);
	result -= n;
	return result;
}

grammar_t::links &grammar_t::iterator::next() const
{
	return ((grammar_t::symbol*)loc)->next;
}

grammar_t::links &grammar_t::iterator::prev() const
{
	return ((grammar_t::symbol*)loc)->prev;
}

bool grammar_t::iterator::operator==(iterator iter) const
{
	return loc == iter.loc;
}

bool grammar_t::iterator::operator!=(iterator iter) const
{
	return loc != iter.loc;
}

bool grammar_t::iterator::operator==(const_iterator iter) const
{
	return loc == iter.loc;
}

bool grammar_t::iterator::operator!=(const_iterator iter) const
{
	return loc != iter.loc;
}

grammar_t::iterator grammar_t::iterator::insert(grammar_t::symbol *sym) const
{
	if (sym->left && sym->left != sym)
		sym->left->right = sym->right;
	if (sym->right && sym->right != sym)
		sym->right->left = sym->left;

	sym->left = loc->left;
	sym->right = loc;
	sym->left->right = sym;
	loc->left = sym;
	return iterator(sym);
}

grammar_t::iterator grammar_t::iterator::link(iterator n) const
{
	next().push_back(n);
	if (n)
		n.prev().push_back(*this);
	return n;
}

grammar_t::links grammar_t::iterator::link(links n) const
{
	for (int i = 0; i < (int)n.size(); i++)
		link(n[i]);
	return n;
}

grammar_t::iterator grammar_t::iterator::rlink(iterator n) const
{
	prev().push_back(n);
	if (n)
		n.next().push_back(*this);
	return n;
}

void grammar_t::iterator::unlink_next(iterator n) const
{
	next().resize(remove(next().begin(), next().end(), n) - next().begin());
}

void grammar_t::iterator::unlink_prev(iterator n) const
{
	prev().resize(remove(prev().begin(), prev().end(), n) - prev().begin());
}

void grammar_t::iterator::unlink(iterator n) const
{
	unlink_next(n);
	if (n)
		n.unlink_prev(*this);
}

void grammar_t::iterator::runlink(iterator n) const
{
	unlink_prev(n);
	if (n)
		n.unlink_next(*this);
}

void grammar_t::iterator::drop()
{
	links n = next();
	links p = prev();
	for (link_iterator i = n.begin(); i != n.end(); i++)
		i->unlink_prev(*this);
	for (link_iterator i = p.begin(); i != p.end(); i++)
		i->unlink_next(*this);
	loc->left->right = loc->right;
	loc->right->left = loc->left;
	node *del = loc;
	loc = loc->left;
	delete del;
}

grammar_t::iterator grammar_t::iterator::push(grammar_t::symbol *sym) const
{
	return link(insert(sym));
}

grammar_t::iterator grammar_t::iterator::rpush(grammar_t::symbol *sym) const
{
	return rlink(insert(sym));
}

grammar_t::iterator &grammar_t::iterator::operator=(const iterator &copy)
{
	loc = copy.loc;
	return *this;
}

/********************** grammar_t::const_iterator **********************/

grammar_t::const_iterator::const_iterator()
{
	loc = NULL;
}

grammar_t::const_iterator::const_iterator(const node *loc)
{
	this->loc = loc;
}

grammar_t::const_iterator::const_iterator(const iterator &copy)
{
	loc = copy.loc;
}

grammar_t::const_iterator::const_iterator(const const_iterator &copy)
{
	loc = copy.loc;
}

grammar_t::const_iterator::operator bool() const
{
	return loc != NULL and loc->left != loc and loc->right != loc;
}

const grammar_t::symbol &grammar_t::const_iterator::operator*() const
{
	return *(const grammar_t::symbol*)loc;
}

const grammar_t::symbol *grammar_t::const_iterator::operator->() const
{
	return (const grammar_t::symbol*)loc;
}

grammar_t::const_iterator grammar_t::const_iterator::operator++(int)
{
	const_iterator result(loc);
	loc = loc->right;
	return result;
}

grammar_t::const_iterator grammar_t::const_iterator::operator--(int)
{
	const_iterator result(loc);
	loc = loc->left;
	return result;
}

grammar_t::const_iterator &grammar_t::const_iterator::operator++()
{
	loc = loc->right;
	return *this;
}

grammar_t::const_iterator &grammar_t::const_iterator::operator--()
{
	loc = loc->left;
	return *this;
}

grammar_t::const_iterator &grammar_t::const_iterator::operator+=(int n)
{
	for (int i = 0; i < n && loc->right != loc; i++)
		loc = loc->right;

	return *this;
}

grammar_t::const_iterator &grammar_t::const_iterator::operator-=(int n)
{
	for (int i = 0; i < n && loc->left != loc; i++)
		loc = loc->left;

	return *this;
}

grammar_t::const_iterator grammar_t::const_iterator::operator+(int n) const
{
	const_iterator result(loc);
	result += n;
	return result;
}

grammar_t::const_iterator grammar_t::const_iterator::operator-(int n) const
{
	const_iterator result(loc);
	result -= n;
	return result;
}

grammar_t::const_links grammar_t::const_iterator::next() const
{
	const_links result;
	result.reserve(((symbol*)loc)->next.size());
	for (int i = 0; i < (int)((symbol*)loc)->next.size(); i++)
		result.push_back(const_iterator(((symbol*)loc)->next[i].loc));
	return result;
}

grammar_t::const_links grammar_t::const_iterator::prev() const
{
	const_links result;
	result.reserve(((symbol*)loc)->prev.size());
	for (int i = 0; i < (int)((symbol*)loc)->prev.size(); i++)
		result.push_back(const_iterator(((symbol*)loc)->prev[i].loc));
	return result;
}

bool grammar_t::const_iterator::operator==(iterator iter) const
{
	return loc == iter.loc;
}

bool grammar_t::const_iterator::operator!=(iterator iter) const
{
	return loc != iter.loc;
}

bool grammar_t::const_iterator::operator==(const_iterator iter) const
{
	return loc == iter.loc;
}

bool grammar_t::const_iterator::operator!=(const_iterator iter) const
{
	return loc != iter.loc;
}

grammar_t::const_iterator &grammar_t::const_iterator::operator=(const const_iterator &copy)
{
	loc = copy.loc;
	return *this;
}

grammar_t::const_iterator &grammar_t::const_iterator::operator=(const iterator &copy)
{
	loc = copy.loc;
	return *this;
}

/********************** grammar_t::rule **********************/

grammar_t::rule::rule()
{
}

grammar_t::rule::rule(std::string name, bool atomic)
{
	this->name = name;
	this->atomic = atomic;
}

grammar_t::rule::~rule()
{
}

void grammar_t::rule::push(const_iterator it)
{
	start.push_back(it);
}

/********************** grammar_t **********************/

grammar_t::grammar_t()
{
	left.right = &right;
	right.left = &left;
}

grammar_t::~grammar_t()
{
	clear();
}

void grammar_t::clear()
{
	node *curr = left.right, *prev;
	while (curr != &right)
	{
		prev = curr;
		curr = curr->right;
		delete prev;
	}
	left.right = &right;
	right.left = &left;
}

grammar_t::iterator grammar_t::insert(symbol *sym)
{
	if (sym->left && sym->left != sym)
		sym->left->right = sym->right;
	if (sym->right && sym->right != sym)
		sym->right->left = sym->left;

	sym->left = right.left;
	sym->right = &right;
	sym->left->right = sym;
	right.left = sym;
	return iterator(sym);
}

int grammar_t::size()
{
	int count = 0;
	for (iterator i = begin(); i != end(); i++)
		++count;
	return count;
}

grammar_t::iterator grammar_t::begin()
{
	return iterator(left.right);
}

grammar_t::iterator grammar_t::rbegin()
{
	return iterator(right.left);
}

grammar_t::iterator grammar_t::end()
{
	return iterator(&right);
}

grammar_t::iterator grammar_t::rend()
{
	return iterator(&left);
}

grammar_t::const_iterator grammar_t::begin() const
{
	return const_iterator(left.right);
}

grammar_t::const_iterator grammar_t::rbegin() const
{
	return const_iterator(right.left);
}

grammar_t::const_iterator grammar_t::end() const
{
	return const_iterator(&right);
}

grammar_t::const_iterator grammar_t::rend() const
{
	return const_iterator(&left);
}

void grammar_t::import(const grammar_t &gram)
{
	std::map<const node*, node*> nodes;

	// Populate the list of nodes
	for (const_iterator i = gram.begin(); i != gram.end(); i++)
	{
		iterator j = insert(i->clone((int)rules.size()));
		nodes.insert(std::pair<const node*, node*>(i.loc, j.loc));
	}

	// Link them all together
	for (std::map<const node*, node*>::iterator m = nodes.begin(); m != nodes.end(); m++)
	{
		const_iterator i(m->first);
		iterator j(m->second);
		const_links ilinks = i.next();
	
		for (const_link_iterator li = ilinks.begin(); li != ilinks.end(); li++)
		{
			std::map<const node*, node*>::iterator n = nodes.find(li->loc);
			if (n != nodes.end())
				j.link(n->second);
			else
				printf("link not found\n");
		}
	}

	// Populate the list of rules
	for (int i = 0; i < (int)gram.rules.size(); i++)
	{
		rules.push_back(rule(gram.rules[i].name, gram.rules[i].atomic));
		for (const_link_const_iterator li = gram.rules[i].start.begin(); li != gram.rules[i].start.end(); li++)
		{
			std::map<const node*, node*>::iterator n = nodes.find(li->loc);
			if (n != nodes.end())
				rules.back().start.push_back(n->second);
			else
				printf("link not found\n");
		}
	}
}

void grammar_t::save(std::string space, std::string name, std::ostream &header, std::ostream &source)
{
	using std::endl;
	
	header << "#pragma once" << endl << endl;
	header << "#include <parse/grammar.h>" << endl << endl;
	if (space.size() > 0) {
		header << "namespace " << space << endl;
		header << "{" << endl << endl;
	}
	header << "struct " << name << "_t : parse::grammar_t" << endl;
	header << "{" << endl;
	header << "\t" << name << "_t();" << endl;
	header << "\t~" << name << "_t();" << endl;
	header << "};" << endl << endl;
	if (space.size() > 0) {
		header << "}" << endl << endl;
	}	

	source << "#include \"" << name << ".h\"" << endl << endl;
	source << "#include <parse/default.h>" << endl << endl;

	if (space.size() > 0) {
		source << "namespace " << space << endl;
		source << "{" << endl << endl;
	}

	source << name << "_t::" << name << "_t()" << endl;
	source << "{" << endl;

	for (int i = 0; i < (int)rules.size(); i++)
		source << "\trules.push_back(rule(\"" << rules[i].name << "\", " << (rules[i].atomic ? "true" : "false") << "));" << endl;
	source << endl;

	source << "\titerator n[" << size() << "];" << endl; 
	int index = 0;
	std::map<const node*, int> indices;
	for (iterator i = begin(); i != end(); i++)
	{
		indices.insert(std::pair<const node*, int>(i.loc, index));
		source << "\tn[" << index++ << "] = insert(new " << i->emit() << ");" << endl;
	}
	source << endl;

	index = 0;
	for (iterator i = begin(); i != end(); i++)
	{
		for (link_iterator j = i.next().begin(); j != i.next().end(); j++)
		{
			if (not *j)
				source << "\tn[" << index << "].link(end());" << endl;
			else
			{
				std::map<const node*, int>::iterator nextindex = indices.find(j->loc);
				if (nextindex != indices.end())
					source << "\tn[" << index << "].link(n[" << nextindex->second << "]);" << endl;
				else
					printf("link not found from %d\n", index);
			}
		}
		++index;
	}
	source << endl;

	for (int i = 0; i < (int)rules.size(); i++)
	{
		for (const_link_iterator j = rules[i].start.begin(); j != rules[i].start.end(); j++)
		{
			if (not *j)
				source << "\trules[" << i << "].start.push_back(end());" << endl;
			else
			{
				std::map<const node*, int>::iterator nextindex = indices.find(j->loc);
				if (nextindex != indices.end())
					source << "\trules[" << i << "].start.push_back(n[" << nextindex->second << "]);" << endl;
				else
					printf("link not found from rule %d\n", i);
			}
		}
	}

	source << "}" << endl << endl;

	source << name << "_t::~" << name << "_t()" << endl;
	source << "{" << endl;
	source << "}" << endl << endl;
	
	if (space.size() > 0) {
		source << "}" << endl << endl;
	}
}

parsing grammar_t::parse(lexer_t &lexer, int index)
{
	parsing best;
	branches_t stack(rules[index], lexer.offset);

	while (stack)
	{
		if (not stack.curr())
			stack.pop_frame(lexer.offset);
		else
		{
			parsing result = stack.curr()->parse(lexer);

			if (result.stem >= 0)
				stack.push_frame(rules[result.stem], lexer.offset, stack.curr()->keep);
			else
			{
				//printf("Parsing: %s\n", result.tree.type.c_str());
				//result.emit(lexer);
				//printf("Done\n");			

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
						//printf("Rewind\n");
						//best.emit(lexer);
						stack.rewind(lexer, &best.msgs);
					}
					else
						stack.rewind(lexer);
				}
				else
				{
					//printf("Advancing\n");
					stack.advance();
				}
			}
		}
	
		//printf("\n");
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
