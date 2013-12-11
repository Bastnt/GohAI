#include "Node.h"
#include <iostream>
#include <list>

using namespace std;

int main() {

	// Test on references
	Node a;
	Node &r = a;
	Node b;
	b.winCounter_ = 2;
	r = b;

	cout << (r.winCounter_) << endl;

	system("PAUSE");

	return 0;
}