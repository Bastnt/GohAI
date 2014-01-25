#include <iostream>
#include <list>
#include "Go.h"

using namespace std;

int main() {
	Go* g = new Go();
	g->GetBestMove(11,0);
	delete g;
	system("PAUSE");
	return 0;
}