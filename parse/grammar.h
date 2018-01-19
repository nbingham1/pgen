#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include <parse/token.h>
#include <parse/message.h>
#include <parse/lexer.h>

namespace parse
{

struct parsing
{
	parsing(intptr_t begin = -1, intptr_t end = -1);
	~parsing();

	int stem;
	token_t tree;
	std::vector<message> msgs;

	void emit(lexer_t &lexer);
};

struct grammar_t
{
	struct iterator;
	struct const_iterator;

	typedef std::vector<iterator> links;
	typedef std::vector<const_iterator> const_links;
	typedef typename std::vector<iterator>::iterator link_iterator;
	typedef typename std::vector<const_iterator>::iterator const_link_iterator;
	typedef typename std::vector<iterator>::const_iterator link_const_iterator;
	typedef typename std::vector<const_iterator>::const_iterator const_link_const_iterator;
		
	struct node
	{
		node();
		virtual ~node();

		node *left;
		node *right;
	};

	struct symbol : node 
	{
		symbol(bool keep = true);
		virtual ~symbol();

		bool keep;
		
		links next;
		links prev;

		virtual parsing parse(lexer_t &lexer) const = 0;
	};

	struct iterator
	{
		friend class grammar_t;
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
		friend class grammar_t;
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
		rule();
		rule(std::string name, bool keep = true);
		~rule();

		std::string name;
		const_links start;
		bool keep;

		void push(const_iterator it);
	};
	
	node left;
	node right;
	std::vector<rule> rules;  

	grammar_t();
	virtual ~grammar_t();

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
};

parsing parse(const grammar_t &grammar, lexer_t &lexer, int index = 0);

}
