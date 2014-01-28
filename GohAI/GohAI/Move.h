#ifndef MOVE
#define MOVE

class Move {
	public:
		char x_, y_;
		Move(char x=-1, char y=-1);
		static const Move GetNeighbour(const Move& move, char indice);
		static const Move GetDiagonal(const Move& move, char indice);
};
#endif