#pragma once
#include <string>
#include <list>

using namespace std;

struct Op {
	virtual list<string> build(unsigned int) = 0;

protected:
	static const string arrow_down;
};

const string Op::arrow_down = "!";

struct Nonterminal : Op {
	Nonterminal(string const& s)
		: value(s)
	{}

	list<string> build(unsigned int) override {
		return { "*", value };
	}

private:
	string value;
};

struct Terminal : Op {
	Terminal(string const& s)
		: value(s)
	{}

	list<string> build(unsigned int) override {
		return { value };
	}

private:
	string value;
};

struct Concat : Op {
	Concat(Op *l, Op *r)
		: left(l)
		, right(r)
	{}

	list<string> build(unsigned int off) override {
		auto t = left->build(off);
		t.splice(t.end(), right->build(off + t.size()));
		return t;
	}

private:
	Op *left;
	Op *right;
};

struct Union : Op {
	Union(Op *l, Op *r)
		: left(l)
		, right(r)
	{}

	list<string> build(unsigned int off) override {
		auto a = left->build(off + 2);
		unsigned int b_off = off + 4 + a.size();
		auto b = right->build(b_off);
		a.splice(a.begin(), { "<", to_string(b_off) });
		a.splice(a.end(), { arrow_down, to_string(b_off + b.size()) });
		a.splice(a.end(), b);
		return a;
	}

private:
	Op *left;
	Op *right;
};

struct Iteration : Op {
	Iteration(Op *l, Op *r)
		: left(l)
		, right(r)
	{}

	list<string> build(unsigned int off) override {
		auto a = left->build(off);
		unsigned int b_off = off + 2 + a.size();
		auto b = right->build(b_off);
		a.splice(a.end(), { "<", to_string(b_off + b.size() + 2) });
		a.splice(a.end(), b);
		a.splice(a.end(), { arrow_down, to_string(off) });
		return a;
	}

private:
	Op *left;
	Op *right;
};