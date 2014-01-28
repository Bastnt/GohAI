#include "Move.h"

Move::Move(char x, char y) : x_(x), y_(y) {}

bool Move::operator== (const Move &m) const { 
	return m.x_==x_ && m.y_==y_; 
}

const Move Move::GetNeighbour(const Move& move, char indice) {
	if (indice == 0) return Move (move.x_ - 1, move.y_);
	if (indice == 1) return Move (move.x_, move.y_ - 1);
	if (indice == 2) return Move (move.x_ + 1, move.y_);
	if (indice == 3) return Move (move.x_, move.y_ + 1);
}

const Move Move::GetDiagonal(const Move& move, char indice) {
	if (indice == 0) return Move (move.x_ - 1, move.y_ - 1);
	if (indice == 1) return Move (move.x_ + 1, move.y_ - 1);
	if (indice == 2) return Move (move.x_ + 1, move.y_ + 1);
	if (indice == 3) return Move (move.x_ - 1, move.y_ + 1);
}