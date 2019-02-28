#pragma comment(linker, "/STACK:1000000000")
#include <iostream>
#include <vector>
#include <string>
#include "builder.h"
#include <fstream>
#include <iomanip>

using namespace std;

void print_array(ostream &os, list<string> const& g) {
	int numsize = 4, boxsize = 9;
	os << setw(numsize) << ' ';
	for (int i = 0; i < 10; ++i)
		os << setw(boxsize) << i;
	os << '\n';

	int count = 0;
	for (auto element : g) {
		if (count % 10 == 0)
			os << setw(numsize) << count / 10;
		os << setw(boxsize) << element;
		if (++count % 10 == 0)
			os << '\n';
	}
}

void linear_graph(vector<string> const& lines, ostream &os) {
	Builder builder;
	if (!builder.read(lines)) {
		cerr << "Incorrect rules\n";
		return;
	}
	list<string> g;
	bool b = builder.go(g);
	if (!b) {
		cerr << "Error parsing rules\n";
		return;
	}

	print_array(os, g);
}

/*
Tests: bigG.txt GTST.txt YardG.txt g0.txt
*/

int main() {
	string filename;
	cout << "Input file: ";
	cin >> filename;
	cout << endl;
	ifstream fin(filename);
	cout << "Output file: ";
	cin >> filename;
	cout << endl;
	ofstream fout(filename);

	int n;
	fin >> n;
	vector<string> lines(n);
	for (int i = 0; i < n; ++i)
		getline(fin >> std::ws, lines[i]);

	linear_graph(lines, fout);

	return 0;
}