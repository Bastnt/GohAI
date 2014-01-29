#include <iostream>
#include <time.h>
#include "GohAI.h"

using namespace std;

int main() {
	GohAI ia;
	ia.Init();
	/*ia.goban_[0][0] = Blanc;
	ia.goban_[0][1] = Blanc;
	ia.goban_[0][2] = Blanc;
	ia.goban_[0][3] = Blanc;
	ia.goban_[0][4] = Blanc;
	ia.goban_[0][5] = Blanc;
	ia.goban_[0][6] = Blanc;
	ia.goban_[0][7] = Blanc;
	ia.goban_[0][8] = Blanc;
	ia.goban_[1][0] = Blanc;
	ia.goban_[1][1] = Blanc;
	ia.goban_[1][2] = Blanc;
	ia.goban_[1][3] = Blanc;
	ia.goban_[1][4] = Blanc;
	ia.goban_[2][5] = Blanc;
	ia.goban_[3][4] = Blanc;
	ia.goban_[3][3] = Blanc;
	ia.goban_[3][5] = Blanc;
	ia.goban_[2][3] = Blanc;

	ia.goban_[4][5] = Noir;
	ia.goban_[4][4] = Noir;
	ia.goban_[5][6] = Noir;
	ia.goban_[5][4] = Noir;
	ia.goban_[5][5] = Noir;
	ia.goban_[6][4] = Noir;
	ia.goban_[6][6] = Noir;*/

	Move m;
	int a,b;
	/*clock_t start = clock();
	for(int i=0; i < 100000; ++i)
		m = ia.NextMove(Blanc);
	cout << "time: " << (clock() - start) << endl;*/
	while(true) {
		m = ia.GetBestMove(5000, Noir);
		ia.Play(m, Noir);
		ia.ReallocateRoot(m);

		cout << "Noir: " << (int)m.x_ << "," << (int)m.y_ << endl;
		ia.DisplayGoban();
		//scanf("%d %d", &a, &b);
		m = ia.GetBestMove(6000, Blanc);
		ia.Play(m, Blanc);
		ia.ReallocateRoot(m);
		cout << "Blanc: " << (int)m.x_ << "," << (int)m.y_ << endl;
		ia.DisplayGoban();
	}

	cin.ignore();
	return 0;
}