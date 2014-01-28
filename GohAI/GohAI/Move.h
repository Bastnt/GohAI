#ifndef MOVE
#define MOVE

/*
	A move, an intersection in the go game
*/
class Move {
	public:
		//Coordonates of the move
		char x_, y_;
		//Default constructor, (-1;-1) means to skip one's turn
		Move(char x=-1, char y=-1);
		//Returns a neighbour of the move, indice defines which one (from 0 to 3)
		static const Move GetNeighbour(const Move& move, char indice);
		//Returns a diagonal of the move, indice defines which one (from 0 to 3)
		static const Move GetDiagonal(const Move& move, char indice);
};
#endif