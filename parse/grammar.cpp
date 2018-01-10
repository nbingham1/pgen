#include "grammar.h"

using namespace std;

int classid_count = 0;

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

void grammar::token::clear()
{
	tokens.clear();
	end = begin;
}

void grammar::token::emit(lex &lexer, string tab)
{
	if (tokens.size() == 0)
		printf("%s:%ld-%ld: \"%s\"\n", type.c_str(), begin, end, lexer.read(begin, end).c_str());
	else
	{
		printf("%s:%ld-%ld: \"%s\"\n{\n", type.c_str(), begin, end, lexer.read(begin, end).c_str());
		for (int i = 0; i < (int)tokens.size(); i++)
			tokens[i].emit(lexer, tab+"\t");
		printf("}\n");
	}
}

/********************** PARSING **********************/

grammar::parsing::parsing(intptr_t begin, intptr_t end) : tree(begin, end) {}
grammar::parsing::~parsing() {}

void grammar::parsing::emit(lex &lexer)
{
	tree.emit(lexer);
	for (int i = 0; i < (int)msgs.size(); i++)
		msgs[i].emit();
}

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

grammar::symbol::symbol() { id = classid<grammar::symbol>(); }
grammar::symbol::~symbol() { }

/********************** LEAF **********************/

grammar::leaf::leaf() { id = classid<grammar::leaf>(); }
grammar::leaf::~leaf() { }

/********************** STEM **********************/

grammar::stem::stem()
{
	id = classid<grammar::stem>();
	reference = -1;
}

grammar::stem::stem(int reference)
{
	id = classid<grammar::stem>();
	this->reference = reference;
}

grammar::stem::~stem()
{
}

int grammar::stem::setup(lex &lexer) const
{
	return reference;
}

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

grammar::iterator grammar::iterator::operator++(int)
{
	iterator result(loc);
	loc = loc->right;
	return result;
}

grammar::iterator grammar::iterator::operator--(int)
{
	iterator result(loc);
	loc = loc->left;
	return result;
}

grammar::iterator &grammar::iterator::operator++()
{
	loc = loc->right;
	return *this;
}

grammar::iterator &grammar::iterator::operator--()
{
	loc = loc->left;
	return *this;
}

grammar::iterator &grammar::iterator::operator+=(int n)
{
	for (int i = 0; i < n && loc->right != loc; i++)
		loc = loc->right;

	return *this;
}

grammar::iterator &grammar::iterator::operator-=(int n)
{
	for (int i = 0; i < n && loc->left != loc; i++)
		loc = loc->left;

	return *this;
}

grammar::iterator grammar::iterator::operator+(int n) const
{
	iterator result(loc);
	result += n;
	return result;
}

grammar::iterator grammar::iterator::operator-(int n) const
{
	iterator result(loc);
	result -= n;
	return result;
}

grammar::links &grammar::iterator::next() const
{
	return ((grammar::symbol*)loc)->next;
}

grammar::links &grammar::iterator::prev() const
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

grammar::iterator grammar::iterator::insert(grammar::symbol *sym) const
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

grammar::iterator grammar::iterator::link(iterator n) const
{
	next().push_back(n);
	if (n)
		n.prev().push_back(*this);
	return n;
}

grammar::links grammar::iterator::link(links n) const
{
	for (int i = 0; i < (int)n.size(); i++)
		link(n[i]);
	return n;
}

grammar::iterator grammar::iterator::rlink(iterator n) const
{
	prev().push_back(n);
	if (n)
		n.next().push_back(*this);
	return n;
}

void grammar::iterator::unlink_next(iterator n) const
{
	next().resize(remove(next().begin(), next().end(), n) - next().begin());
}

void grammar::iterator::unlink_prev(iterator n) const
{
	prev().resize(remove(prev().begin(), prev().end(), n) - prev().begin());
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

grammar::iterator grammar::iterator::push(grammar::symbol *sym) const
{
	return link(insert(sym));
}

grammar::iterator grammar::iterator::rpush(grammar::symbol *sym) const
{
	return rlink(insert(sym));
}

grammar::iterator &grammar::iterator::operator=(const iterator &copy)
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

grammar::const_iterator::const_iterator(const iterator &copy)
{
	loc = copy.loc;
}

grammar::const_iterator::const_iterator(const const_iterator &copy)
{
	loc = copy.loc;
}

grammar::const_iterator::operator bool() const
{
	return loc != NULL and loc->left != loc and loc->right != loc;
}

const grammar::symbol &grammar::const_iterator::operator*() const
{
	return *(const grammar::symbol*)loc;
}

const grammar::symbol *grammar::const_iterator::operator->() const
{
	return (const grammar::symbol*)loc;
}

grammar::const_iterator grammar::const_iterator::operator++(int)
{
	const_iterator result(loc);
	loc = loc->right;
	return result;
}

grammar::const_iterator grammar::const_iterator::operator--(int)
{
	const_iterator result(loc);
	loc = loc->left;
	return result;
}

grammar::const_iterator &grammar::const_iterator::operator++()
{
	loc = loc->right;
	return *this;
}

grammar::const_iterator &grammar::const_iterator::operator--()
{
	loc = loc->left;
	return *this;
}

grammar::const_iterator &grammar::const_iterator::operator+=(int n)
{
	for (int i = 0; i < n && loc->right != loc; i++)
		loc = loc->right;

	return *this;
}

grammar::const_iterator &grammar::const_iterator::operator-=(int n)
{
	for (int i = 0; i < n && loc->left != loc; i++)
		loc = loc->left;

	return *this;
}

grammar::const_iterator grammar::const_iterator::operator+(int n) const
{
	const_iterator result(loc);
	result += n;
	return result;
}

grammar::const_iterator grammar::const_iterator::operator-(int n) const
{
	const_iterator result(loc);
	result -= n;
	return result;
}

grammar::const_links grammar::const_iterator::next() const
{
	const_links result;
	result.reserve(((symbol*)loc)->next.size());
	for (int i = 0; i < (int)((symbol*)loc)->next.size(); i++)
		result.push_back(const_iterator(((symbol*)loc)->next[i].loc));
	return result;
}

grammar::const_links grammar::const_iterator::prev() const
{
	const_links result;
	result.reserve(((symbol*)loc)->prev.size());
	for (int i = 0; i < (int)((symbol*)loc)->prev.size(); i++)
		result.push_back(const_iterator(((symbol*)loc)->prev[i].loc));
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

grammar::const_iterator &grammar::const_iterator::operator=(const const_iterator &copy)
{
	loc = copy.loc;
	return *this;
}

grammar::const_iterator &grammar::const_iterator::operator=(const iterator &copy)
{
	loc = copy.loc;
	return *this;
}

/********************** FORK **********************/

grammar::fork::fork() {}

grammar::fork::fork(int frame, const grammar &gram, int index)
{
	rule = index;
	parent = frame;
	branches = gram.rules[index].start;
	branch = 0;
	curr = branches[branch];
}

grammar::fork::fork(int frame, const_iterator point)
{
	rule = -1;
	parent = frame;
	if (point)
	{
		branches = point.next();
		branch = 0;
		curr = branches[branch];
	}
	else
	{
		branch = 0;
		curr = point;
	}
}

grammar::fork::~fork() {}

/********************** FORKS **********************/

grammar::forks::forks()
{
	frame = -1;
}

grammar::forks::forks(const grammar &gram, int index)
{
	elems.push_back(fork(-1, gram, index));
	frame = 0;
}

grammar::forks::~forks()
{
}

grammar::forks::operator bool()
{
	return (int)elems.size() > 0 && (elems.back().curr || frame >= 0);
}

void grammar::forks::push(const_iterator point)
{
	elems.push_back(fork(frame, point));
}

void grammar::forks::pop()
{
	elems.pop_back();
}

void grammar::forks::push_frame(const grammar &gram, int index)
{
	elems.push_back(fork(frame, gram, index));
	frame = (int)elems.size()-1;
}

void grammar::forks::pop_frame()
{
	if (frame > 0)
		elems.push_back(fork(elems[frame].parent, elems[frame-1].curr));
	else
		elems.push_back(fork(elems[frame].parent, const_iterator()));
	frame = elems.back().parent;
}

bool grammar::forks::rewind(const grammar &gram, lex &lexer, vector<message> *msgs)
{
	printf("rewind: %d, %d/%d\n", (int)elems.size(), elems.back().branch, (int)elems.back().branches.size());

	if (elems.back().branch < (int)elems.back().branches.size())
	{
		++(elems.back().branch);
		while ((int)elems.size() > 1 and elems.back().branch >= (int)elems.back().branches.size())
		{
			if (msgs != NULL and elems.back().rule >= 0)
			{
				msgs->push_back(message(message::error, "expected " + gram.rules[elems.back().rule].name, lexer, true, elems.back().tree.begin, elems.back().tree.end));
			}
			elems.pop_back();
			++(elems.back().branch);
		}
	}

	if (elems.back().branch >= (int)elems.back().branches.size())
	{
		printf("pop_back\n");
		elems.pop_back();
		return false;
	}
	else
	{
		printf("moving\n");
		elems.back().tree.emit(lexer);
		lexer.moveto(elems.back().tree.begin-1);
		elems.back().tree.clear();
		elems.back().curr = elems.back().branches[elems.back().branch];
		if (elems.back().rule >= 0)
			frame = (int)elems.size()-1;
		else
			frame = elems.back().parent;
		return true;
	}
}

void grammar::forks::advance()
{
	if ((int)elems.back().curr.next().size() == 0)
		elems.back().curr.loc = NULL;
	else if ((int)elems.back().curr.next().size() == 1)
		elems.back().curr = elems.back().curr.next()[0];
	else
		elems.push_back(fork(frame, elems.back().curr));
}

grammar::fork &grammar::forks::back()
{
	return elems.back();
}

grammar::const_iterator &grammar::forks::curr()
{
	return elems.back().curr;
}

grammar::token grammar::forks::collapse_frame(int &index)
{
	int start = index;
	token result;
	while (index < (int)elems.size())
	{
		if (index == start)
			result = elems[index++].tree;
		else if (elems[index].parent == start)
		{
			if (elems[index].rule >= 0)
				result.append(collapse_frame(index));
			else
				result.extend(elems[index++].tree);
		}
		else
			return result;
	}
	return result;
}

grammar::token grammar::forks::collapse()
{
	int index = 0;
	return collapse_frame(index);
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

grammar::iterator grammar::insert(symbol *sym)
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

grammar::iterator grammar::begin()
{
	return iterator(left.right);
}

grammar::iterator grammar::rbegin()
{
	return iterator(right.left);
}

grammar::iterator grammar::end()
{
	return iterator(&right);
}

grammar::iterator grammar::rend()
{
	return iterator(&left);
}

grammar::const_iterator grammar::begin() const
{
	return const_iterator(left.right);
}

grammar::const_iterator grammar::rbegin() const
{
	return const_iterator(right.left);
}

grammar::const_iterator grammar::end() const
{
	return const_iterator(&right);
}

grammar::const_iterator grammar::rend() const
{
	return const_iterator(&left);
}

grammar::parsing grammar::parse(lex &lexer, int index)
{
	parsing best;
	forks stack(*this, index);

	while (stack)
	{
		if (not stack.curr())
			stack.pop_frame();
		else if (stack.curr()->is<stem>())
		{
			int rule = stack.curr()->as<stem>()->setup(lexer);
			stack.push_frame(*this, rule);
		}
		else
		{
			parsing result;
			if (stack.curr()->is<leaf>())
				result = stack.curr()->as<leaf>()->parse(lexer);
			else if (not stack.curr()->is<symbol>())
				result.msgs.push_back(message(message::fail, "symbols must inherit from either leaf or stem.", lexer, false, result.tree.begin, result.tree.end));

			printf("Parsing: \n");
			result.emit(lexer);
			printf("Done\n");			

			stack.back().tree.append(result.tree);
			if ((int)result.msgs.size() > 0)
			{
				printf("rewind\n");
				if (result.tree.end > best.tree.end)
				{
					best.tree = stack.collapse();
					best.msgs = result.msgs;
					printf("Best\n");
					best.emit(lexer);
					stack.rewind(*this, lexer, &best.msgs);
				}
				else
					stack.rewind(*this, lexer);
			}
			else
			{
				printf("advancing\n");
				stack.advance();
			}
		}
	
		printf("\n");
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

class_t::class_t()
{
	invert = false;
}

class_t::~class_t()
{
}

void class_t::add(char start)
{
	ranges.push_back(pair<char, char>(start, start));
}

void class_t::add(char start, char end)
{
	ranges.push_back(pair<char, char>(start, end));
}

string class_t::name() const
{
	string result;
	result.push_back('[');
	if (invert)
		result.push_back('^');
	for (int i = 0; i < (int)ranges.size(); i++)
	{
		if (ranges[i].first == ranges[i].second)
			result.push_back(ranges[i].first);
		else {
			result.push_back(ranges[i].first);
			result.push_back('-');
			result.push_back(ranges[i].second);
		}
	}
	result.push_back(']');
	return result;
}

grammar::parsing class_t::parse(lex &lexer) const
{
	char c = lexer.get();
	printf("%c %c %c %ld\n", c, lexer.prev, lexer.curr, lexer.offset);
	grammar::parsing result(lexer.offset);
	++result.tree.end;
	result.tree.type = "class";

	bool match = false;
	for (int i = 0; i < (int)ranges.size() and not match; i++)
		if (c >= ranges[i].first && c <= ranges[i].second)
			match = true;

	if (match == invert)
		result.msgs.push_back(message(message::error, string("expected ") + name() + " but found '" + c + "'.", lexer, true, result.tree.begin, result.tree.end));
	
	return result;
}

