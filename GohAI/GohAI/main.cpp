#include <iostream>
#include <time.h>
#include "GohAI.h"

using namespace std;

void DisplayGoban(vector<vector<char>> goban_) {
	cout << "Goban:" << endl;
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			switch(goban_[i][j]) {
			case Vide: 
				cout << "-";
				break;
			case Blanc: 
				cout << "B";
				break;
			case Noir: 
				cout << "N";
				break;
			}
		}
		cout << endl;
	}
}

int main() {
	GohAI ia;
	ia.Init();
	ia.goban_[0][0] = Blanc;
	ia.goban_[0][1] = Blanc;
	ia.goban_[0][2] = Blanc;
	ia.goban_[0][3] = Blanc;
	ia.goban_[0][4] = Blanc;
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
	DisplayGoban(ia.goban_);
	ia.moves_.push_back(Move(-1,-1));
	ia.moves_.push_back(Move(-1,-1));
	ia.moves_.push_back(Move(-1,-1));
	Move m;
	/*clock_t start = clock();
	for(int i=0; i < 100000; ++i)
		m = ia.NextMove(Blanc);
	cout << "time: " << (clock() - start) << endl;*/
	m = ia.GetBestMove(100, Noir);
	cout << "result: " << (int)m.x_ << "," << (int)m.y_ << endl;

	cout << "Hello World" << endl;
	cin.ignore();
	return 0;
}