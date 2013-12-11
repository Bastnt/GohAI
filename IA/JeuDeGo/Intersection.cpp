#include "Intersection.h"

Intersection::Intersection (int x, int y) : x_(x), y_(y) {}

Intersection Intersection::voisine (int indice) {
	if (indice == 0) return Intersection (x_ - 1, y_);
	if (indice == 1) return Intersection (x_, y_ - 1);
	if (indice == 2) return Intersection (x_ + 1, y_);
	if (indice == 3) return Intersection (x_, y_ + 1);
}


Intersection Intersection::diagonale(int indice) {
	if (indice == 0) return Intersection (x_ - 1, y_ - 1);
	if (indice == 1) return Intersection (x_ + 1, y_ - 1);
	if (indice == 2) return Intersection (x_ + 1, y_ + 1);
	if (indice == 3) return Intersection (x_ - 1, y_ + 1);
}