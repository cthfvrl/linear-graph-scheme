#pragma once
#include "op.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <list>

using namespace std;

class Builder {
public:
	bool read(vector<string> const& lines) {
		for (auto const& rule : lines) {
			auto sz = rule.find(':');
			if (sz == string::npos)
				return false;

			string rulename = rule.substr(0, sz);
			nonterminals[rulename] = -1;
			if (rule.size() <= sz + 2 || rule[sz + 1] != ' ' || rule[sz + 2] == ' ')
				return false;

			rules.push_back(make_pair(rulename, rule.substr(sz + 2)));
		}
		return true;
	}

	bool go(list<string> &g) {
		// Assume start rule to be the 1st one
		g = { ":" };
		// off should start from 1 if : is added at the start
		// otherwise 0
		for (int i = 0, off = 1; i < rules.size(); ++i) {
			Op *tree = nullptr;
			if (!build(rules[i].second, tree))
				return false;

			auto t = tree->build(off);
			//t.push_back(".(Ex" + rules[i].first + ")");
			t.push_back("." + rules[i].first);
			auto delta = t.size();
			g.splice(g.end(), t);
			nonterminals[rules[i].first] = off;
			off += delta;
		}

		bool call = false;
		for (auto it = g.begin(); it != g.end(); ++it) {
			if (call) {
				(*it) = to_string(nonterminals[*it]);
				call = false;
			}
			else if (*it == "*") call = true;
		}

		return true;
	}

private:
	vector<pair<string, string> > rules;
	unordered_map<string, int> nonterminals;

	int priority(char op) {
		switch (op) {
		case '#':
			return 3;
		case ',':
			return 2;
		case ';':
			return 1;
		default:
			return 0;
		}
	}

	bool isop(char c) {
		return c == '#' || c == ',' || c == ';';
	}

	bool isterminal(string const& s) {
		return nonterminals.find(s) == nonterminals.end();
	}

	void process(stack<Op *> &args, char op) {
		auto right = args.top();
		args.pop();
		auto left = args.top();
		args.pop();
		switch (op) {
		case '#':
			args.push(new Iteration(left, right));
			break;
		case ',':
			args.push(new Concat(left, right));
			break;
		case ';':
			args.push(new Union(left, right));
			break;
		default:
			throw 1;
		}
	}

	bool build(string const& s, Op* &result) {
		stack<char> ops;
		stack<Op *> args;

		for (int i = 0; i < s.size(); ++i) {
			char c = s[i];
			if (c == '.')
				break;
			if (c == '(')
				ops.push(c);
			else if (c == ')') {
				while (ops.top() != '(') {
					process(args, ops.top());
					ops.pop();
				}
				ops.pop();
			}
			else if (isop(c)) {
				while (!ops.empty() && priority(ops.top()) >= priority(c)) {
					process(args, ops.top());
					ops.pop();
				}
				ops.push(c);
			}
			else {
				stringstream ss;
				ss << c;
				if (c == '\'') {
					for (++i; i < s.size() && s[i] != '\''; ++i) {
						if (s[i] == ' ') return false;
						ss << s[i];
					}
					if (i == s.size()) return false;
					ss << s[i];
				}
				else {
					for (++i; i < s.size() && s[i] != '.' && s[i] != '(' && s[i] != ')' && !isop(s[i]); ++i) {
						if (s[i] == ' ') return false;
						ss << s[i];
					}
					--i;
				}
				auto name = ss.str();
				if (isterminal(name))
					args.push(new Terminal(name));
				else
					args.push(new Nonterminal(name));
			}
		}

		while (!ops.empty()) {
			process(args, ops.top());
			ops.pop();
		}

		result = args.top();
		return true;
	}
};

