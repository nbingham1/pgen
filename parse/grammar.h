#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "message.h"
#include "lex.h"

extern int classid_count;

template <typename T>
int classid(T *inst = (T*)0)
{
	static int id = classid_count++;
	return id;
}

struct grammar
{
	struct iterator;
	struct const_iterator;

	typedef vector<iterator> links;
	typedef vector<const_iterator> const_links;
	typedef typename vector<iterator>::iterator link_iterator;
	typedef typename vector<const_iterator>::iterator const_link_iterator;
	typedef typename vector<iterator>::const_iterator link_const_iterator;
	typedef typename vector<const_iterator>::const_iterator const_link_const_iterator;

	struct token
	{
		token(intptr_t begin = -1, intptr_t end = -1);
		~token();

		string type;
		intptr_t begin;
		intptr_t end;
		vector<token> tokens;

		operator bool();
		void append(const token &t);
		void extend(const token &t);
		void clear();
		
		void emit(lex &lexer, string tab = "");
	};

	struct parsing
	{
		parsing(intptr_t begin = -1, intptr_t end = -1);
		~parsing();

		token tree;
		vector<message> msgs;
	
		void emit(lex &lexer);
	};

	struct node
	{
		node();
		virtual ~node();

		node *left;
		node *right;
	};

	struct symbol : node 
	{
		symbol();
		virtual ~symbol();

		int id;
		links next;
		links prev;

		template <typename T>
		bool is(T *value = (T*)0) const
		{
			return id == classid<T>();
		}

		template <typename T>
		T* as(T *value = (T*)0)
		{
			return (T*)this;
		}

		template <typename T>
		const T* as(T *value = (T*)0) const
		{
			return (const T*)this;
		}
	};

	struct leaf : symbol
	{
		leaf();
		virtual ~leaf();

		virtual parsing parse(lex &lexer) const = 0;
	};

	struct stem : symbol
	{
		stem();
		stem(int reference);
		virtual ~stem();

		int reference;

		virtual int setup(lex &lexer) const;
	};

	struct iterator
	{
		friend class grammar;
		friend class const_iterator;
		
		node *loc;
		
		iterator();
		iterator(node *loc);
		iterator(const iterator &copy);
		operator bool() const;
		symbol &operator*() const;
		symbol *operator->() const;
		iterator operator++(int);
		iterator operator--(int);
		iterator &operator++();
		iterator &operator--();
		iterator &operator+=(int n);
		iterator &operator-=(int n);
		iterator operator+(int n) const;
		iterator operator-(int n) const;
		links &next() const;
		links &prev() const;
		bool operator==(iterator iter) const;
		bool operator!=(iterator iter) const;
		bool operator==(const_iterator iter) const;
		bool operator!=(const_iterator iter) const;
		iterator insert(symbol *sym) const;
		iterator link(iterator n) const;
		links link(links n) const;
		iterator rlink(iterator n) const;
		void unlink_next(iterator n) const;
		void unlink_prev(iterator n) const;
		void unlink(iterator n) const;
		void runlink(iterator n) const;
		void drop();
		iterator push(symbol *sym) const;
		iterator rpush(symbol *sym) const;
		iterator &operator=(const iterator &copy);
	};

	struct const_iterator
	{
		friend class grammar;
		friend class iterator;
		
		const node *loc;

		const_iterator();
		const_iterator(const node *loc);
		const_iterator(const iterator &copy);
		const_iterator(const const_iterator &copy);
		
		operator bool() const;
		const symbol &operator*() const;
		const symbol *operator->() const;

		const_iterator operator++(int);
		const_iterator operator--(int);
		const_iterator &operator++();
		const_iterator &operator--();
		const_iterator &operator+=(int n);
		const_iterator &operator-=(int n);
		const_iterator operator+(int n) const;
		const_iterator operator-(int n) const;
		const_links next() const;
		const_links prev() const;
		bool operator==(iterator iter) const;
		bool operator!=(iterator iter) const;
		bool operator==(const_iterator iter) const;
		bool operator!=(const_iterator iter) const;
		const_iterator &operator=(const const_iterator &copy);
		const_iterator &operator=(const iterator &copy);
	};

	struct rule
	{
		string name;
		bool keep;
		vector<const_iterator> start;
	};
	
	struct fork
	{
		fork();
		fork(int frame, const grammar &gram, int index = 0);
		fork(int frame, const_iterator point);
		~fork();

		int rule;
		int parent;
		token tree;
		const_iterator curr;
		int branch;
		const_links branches;
	};

	struct forks
	{
		forks();
		forks(const grammar &gram, int index = 0);
		~forks();

		vector<fork> elems;
		int frame;

		operator bool();
		void push(const_iterator point);
		void pop();
		
		void push_frame(const grammar &gram, int index = 0);
		void pop_frame();
		bool rewind(const grammar &gram, lex &lexer, vector<message> *msgs = NULL);
		void advance();

		fork &back();
		const_iterator &curr();

		token collapse_frame(int &index);
		token collapse();
	};

	node left;
	node right;
	vector<rule> rules;  

	grammar();
	virtual ~grammar();

	void clear();
	iterator insert(symbol *sym);

	iterator begin();
	iterator rbegin();
	iterator end();
	iterator rend();

	const_iterator begin() const;
	const_iterator rbegin() const;
	const_iterator end() const;
	const_iterator rend() const;

	parsing parse(lex &lexer, int index = 0);
};

struct class_t : grammar::leaf
{
	class_t();
	~class_t();

	vector<pair<char, char> > ranges;
	bool invert;

	void add(char start);
	void add(char start, char end);

	string name() const;
	grammar::parsing parse(lex &lexer) const;
};

