#include "grammar.h"

/********************** TOKEN **********************/

grammar::token::token(intptr_t begin, intptr_t end)
{
	this->begin = begin;
	this->end = end >= 0 ? end : begin;
}

grammar::token::~token() { }

grammar::token::operator bool()
{
	return begin >= 0 and end >= 0 and begin != end;
}

void grammar::token::append(const token &t)
{
	if (begin < 0 or (t.begin >= 0 and begin > t.begin))
		begin = t.begin;
	if (end < 0 or (t.end >= 0 and end < t.end))
		end = t.end;
	tokens.push_back(t);
}

void grammar::token::extend(const token &t)
{
	if (begin < 0 or (t.begin >= 0 and begin > t.begin))
		begin = t.begin;
	if (end < 0 or (t.end >= 0 and end < t.end))
		end = t.end;
	tokens.insert(tokens.end(), t.tokens.begin(), t.tokens.end());
}

/********************** PARSING **********************/

grammar::parsing::parsing(intptr_t begin, intptr_t end) : tree(begin, end) {}
grammar::parsing::~parsing() {}

/********************** NODE **********************/

grammar::node::node()
{
	left = this;
	right = this;
}

grammar::node::~node()
{
}

/********************** SYMBOL **********************/

grammar::symbol::symbol() { id = typeid(grammar::symbol); }
grammar::symbol::~symbol() { }

/********************** LEAF **********************/

grammar::leaf::leaf() { id = typeid(grammar::leaf); }
grammar::leaf::~leaf() { }

/********************** STEM **********************/

grammar::stem::stem() { id = typeid(grammar::stem); }
grammar::stem::~stem() { }

/********************** ITERATOR **********************/

grammar::iterator::iterator()
{
	loc = NULL;
}

grammar::iterator::iterator(node *loc)
{
	this->loc = loc;
}

grammar::iterator::iterator(const iterator &copy)
{
	loc = copy.loc;
}

grammar::iterator::operator bool() const
{
	return loc != NULL and loc->left != loc and loc->right != loc;
}

grammar::symbol &grammar::iterator::operator*() const
{
	return *(grammar::symbol*)loc;
}

grammar::symbol *grammar::iterator::operator->() const
{
	return (grammar::symbol*)loc;
}

iterator grammar::iterator::operator++(int)
{
	iterator result(loc);
	loc = loc->right;
	return result;
}

iterator grammar::iterator::operator--(int)
{
	iterator result(loc);
	loc = loc->left;
	return result;
}

iterator &grammar::iterator::operator++()
{
	loc = loc->right;
	return *this;
}

iterator &grammar::iterator::operator--()
{
	loc = loc->left;
	return *this;
}

iterator &grammar::iterator::operator+=(int n)
{
	for (int i = 0; i < n && loc->right != loc; i++)
		loc = loc->right;

	return *this;
}

iterator &grammar::iterator::operator-=(int n)
{
	for (int i = 0; i < n && loc->left != loc; i++)
		loc = loc->left;

	return *this;
}

iterator grammar::iterator::operator+(int n) const
{
	iterator result(loc);
	result += n;
	return result;
}

iterator grammar::iterator::operator-(int n) const
{
	iterator result(loc);
	result -= n;
	return result;
}

links &grammar::iterator::next() const
{
	return ((grammar::symbol*)loc)->next;
}

links &grammar::iterator::prev() const
{
	return ((grammar::symbol*)loc)->prev;
}

bool grammar::iterator::operator==(iterator iter) const
{
	return loc == iter.loc;
}

bool grammar::iterator::operator!=(iterator iter) const
{
	return loc != iter.loc;
}

bool grammar::iterator::operator==(const_iterator iter) const
{
	return loc == iter.loc;
}

bool grammar::iterator::operator!=(const_iterator iter) const
{
	return loc != iter.loc;
}

iterator grammar::iterator::insert(grammar::symbol *sym)
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

iterator grammar::iterator::link(iterator n) const
{
	next().push_back(n);
	if (n)
		n.prev().push_back(*this);
	return n;
}

links grammar::iterator::link(links n) const
{
	for (int i = 0; i < n.size(); i++)
		link(n[i]);
	return n;
}

iterator grammar::iterator::rlink(iterator n) const
{
	prev().push_back(n);
	if (n)
		n.next().push_back(*this);
	return n;
}

void grammar::iterator::unlink_next(iterator n) const
{
	next().resize(remove(next().begin(), next.end(), n) - next().begin());
}

void grammar::iterator::unlink_prev(iterator n) const
{
	prev().resize(remove(prev().begin(), prev.end(), n) - prev().begin());
}

void grammar::iterator::unlink(iterator n) const
{
	unlink_next(n);
	if (n)
		n.unlink_prev(*this);
}

void grammar::iterator::runlink(iterator n) const
{
	unlink_prev(n);
	if (n)
		n.unlink_next(*this);
}

void grammar::iterator::drop()
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

iterator grammar::iterator::push(grammar::symbol *sym) const
{
	return link(insert(sym));
}

iterator grammar::iterator::rpush(grammar::symbol *sym) const
{
	return rlink(insert(sym));
}

iterator &grammar::iterator::operator=(const iterator &copy)
{
	loc = copy.loc;
	return *this;
}

/********************** CONST_ITERATOR **********************/

grammar::const_iterator::const_iterator()
{
	loc = NULL;
}

grammar::const_iterator::const_iterator(const node *loc)
{
	this->loc = loc;
}

grammar::const_iterator::const_iterator(const const_iterator &copy)
{
	loc = copy.loc;
}

grammar::const_iterator::operator bool() const
{
	return loc->left != loc && loc->right != loc;
}

const grammar::symbol &grammar::const_iterator::operator*() const
{
	return *(const grammar::symbol*)loc;
}

const grammar::symbol *grammar::const_iterator::operator->() const
{
	return (const grammar::symbol*)loc;
}

const_iterator grammar::const_iterator::operator++(int)
{
	const_iterator result(loc);
	loc = loc->right;
	return result;
}

const_iterator grammar::const_iterator::operator--(int)
{
	const_iterator result(loc);
	loc = loc->left;
	return result;
}

const_iterator &grammar::const_iterator::operator++()
{
	loc = loc->right;
	return *this;
}

const_iterator &grammar::const_iterator::operator--()
{
	loc = loc->left;
	return *this;
}

const_iterator &grammar::const_iterator::operator+=(int n)
{
	for (int i = 0; i < n && loc->right != loc; i++)
		loc = loc->right;

	return *this;
}

const_iterator &grammar::const_iterator::operator-=(int n)
{
	for (int i = 0; i < n && loc->left != loc; i++)
		loc = loc->left;

	return *this;
}

const_iterator grammar::const_iterator::operator+(int n) const
{
	const_iterator result(loc);
	result += n;
	return result;
}

const_iterator grammar::const_iterator::operator-(int n) const
{
	const_iterator result(loc);
	result -= n;
	return result;
}

const_links grammar::const_iterator::next() const
{
	const_links result;
	result.reserve(((symbol*)loc)->next.size());
	for (int i = 0; i < ((symbol*)loc)->next.size(); i++)
		result.push_back_unsafe(const_iterator(((symbol*)loc)->next[i].loc));
	return result;
}

const_links grammar::const_iterator::prev() const
{
	const_links result;
	result.reserve(((symbol*)loc)->prev.size());
	for (int i = 0; i < ((symbol*)loc)->prev.size(); i++)
		result.push_back_unsafe(const_iterator(((symbol*)loc)->prev[i].loc));
	return result;
}

bool grammar::const_iterator::operator==(iterator iter) const
{
	return loc == iter.loc;
}

bool grammar::const_iterator::operator!=(iterator iter) const
{
	return loc != iter.loc;
}

bool grammar::const_iterator::operator==(const_iterator iter) const
{
	return loc == iter.loc;
}

bool grammar::const_iterator::operator!=(const_iterator iter) const
{
	return loc != iter.loc;
}

const_iterator &grammar::const_iterator::operator=(const const_iterator &copy)
{
	loc = copy.loc;
	return *this;
}

const_iterator &grammar::const_iterator::operator=(const iterator &copy)
{
	loc = copy.loc;
	return *this;
}

/********************** FORK **********************/

grammar::fork::fork() {}

grammar::fork::fork(const grammar &gram, int index)
{
	rule = index;
	branch = gram.rule[rule].start.begin();
	curr = *brach;
}

grammar::fork::fork(const_iterator point, int index)
{
	rule = -1;
	branch = point.next().begin() + index;
	curr = *branch;
}

grammar::fork::~fork() {}

/********************** FORKS **********************/

grammar::forks::forks() {}

grammar::forks::forks(const grammar &gram, int index)
{
	elems.push_back(fork(gram, index));
}

grammar::forks::~forks() {}

grammar::forks::operator bool()
{
	return elems.size() > 0;
}

void grammar::forks::push(const grammar &gram, int index)
{
	elems.push_back(fork(gram, index));
	frames.push_back(elems.size()-1);
}

void grammar::forks::push(const_iterator point, int index)
{
	elems.push_back(fork(point, index));
}

void grammar::forks::pop()
{
	elems.pop_back();
	while (frames.back() >= elems.size())
		frames.pop_back();
}

fork &grammar::forks::inc()
{
	++(elems.back().branch);
	while (elems.size() > 0 and elems.back().branch == elems.back().something.end())
	{
		elems.pop_back();
		++(elems.back().branch);
	}

	elems.back().curr = *elems.back().branch;
	return elems.back();
}

fork &grammar::forks::seq()
{
	if (elems.back().curr.next().size() == 0)
		elems.back().curr.loc = NULL;
	else if (elems.back().curr.next().size() == 1)
		elems.back().curr = elems.back().curr.next()[0];
	else
		elems.push_back(fork(elems.back().curr));

	return elems.back();
}

fork &grammar::forks::back()
{
	return elems.back();
}

const_iterator &grammar::forks::curr()
{
	return elems.back().curr;
}

token grammar::forks::collapse()
{
}

/********************** GRAMMAR **********************/

grammar::grammar()
{
	left.right = &right;
	right.left = &left;
}

grammar::~grammar()
{
	clear();
}

void grammar::clear()
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

iterator grammar::insert(symbol *sym)
{
	if (sym->left && sym->left != sym)
		sym->left->right = sym->right;
	if (sym->right && sym->right != sym)
		sym->right->left = sym->left;

	sym->left = right.left;
	sym->right = right;
	sym->left->right = sym;
	right->left = sym;
	return iterator(sym);
}

iterator grammar::begin()
{
	return iterator(left.right);
}

iterator grammar::rbegin()
{
	return iterator(right.left);
}

iterator grammar::end()
{
	return iterator(&right);
}

iterator grammar::rend()
{
	return iterator(&left);
}

const_iterator grammar::begin() const
{
	return const_iterator(left.right);
}

const_iterator grammar::rbegin() const
{
	return const_iterator(right.left);
}

const_iterator grammar::end() const
{
	return const_iterator(&right);
}

const_iterator grammar::rend() const
{
	return const_iterator(&left);
}

parsing grammar::parse(lex &lexer, int index)
{
	parsing best(lexer.offset);
	forks stack(*this, index);

	while (stack)
	{
		if (stack.curr() == end())
		{
			
		}
		else if (stack.curr()->is<stem>())
		{
			int rule = stack.curr()->as<stem>()->setup(lexer);
			stack.push(*this, rule);
		}
		else
		{
			parsing result(lexer.offset);
			if (stack.curr()->is<leaf>())
				result = stack.curr()->as<leaf>()->parse(lexer);
			else if (not stack.curr()->is<symbol>())
				result.msgs.push_back(message(message::fail, "symbols must inherit from either leaf or stem."));

			stack.back().tree.append(result);
			if (result.msgs.size() > 0)
			{
				if (result.tree.end > best.tree.end)
				{
					best.tree = stack.collapse();
					best.msgs = result.msgs;
				}

				stack.inc();
			}
			else
				stack.seq();
		}
	}
}

