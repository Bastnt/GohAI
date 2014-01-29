#include <iostream>
#include <time.h>
#include "GohAI.h"

using namespace std;

int main() {
	GohAI ia;
	ia.Init();
	Move m;
	int a,b;
	while(true) {

		m = ia.GetBestMove(10000, Noir);
		ia.Play(m, Noir);
		ia.ReallocateRoot(m);
		cout << "Noir: " << (int)m.x_ << "," << (int)m.y_ << endl;
		ia.DisplayGoban();

		cout << "Blanc: ";
		scanf("%d %d", &a, &b);
		ia.Play(Move(a,b), Blanc);
		ia.ReallocateRoot(Move(a,b));
		ia.DisplayGoban();

		
	}

	cin.ignore();
	return 0;
}