#include <parse/grammar.h>
#include <parse/branch.h>

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

grammar_t::rule::rule(std::string name, bool keep)
{
	this->name = name;
	this->keep = keep;
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

parsing parse(const grammar_t &grammar, lexer_t &lexer, int index)
{
	parsing best;
	branches_t stack(grammar.rules[index], lexer.offset);

	while (stack)
	{
		if (not stack.curr())
			stack.pop_frame(lexer.offset);
		else
		{
			parsing result = stack.curr()->parse(lexer);

			if (result.stem >= 0)
				stack.push_frame(grammar.rules[result.stem], lexer.offset, stack.curr()->keep and grammar.rules[result.stem].keep);
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
						best.tree = stack.collapse();
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
		result.tree = stack.collapse();
		return result;
	}
}

}
