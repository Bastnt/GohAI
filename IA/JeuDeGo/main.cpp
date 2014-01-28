#include <iostream>
#include <list>
#include "Go.h"

using namespace std;

int main() {
	Go* g = new Go();
	g->GetBestMove(5000,0);
	vector<vector<int>> mat;

	cout << "MonteCarlo runs: " << g->Mmc << endl;
	cout << "Select: " << g->Mbm[select] << endl;
	cout << "Expand: " << g->Mbm[expand] << endl;
	cout << "Simulate: " << g->Mbm[simulate] << endl;
	cout << "BackPropage: " << g->Mbm[backpropage] << endl;
	cout << "Playouts: " << g->Mbm[playouts] << endl;
	delete g;
	system("PAUSE");
	return 0;
}