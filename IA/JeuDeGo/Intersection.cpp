#include "Intersection.h"

Intersection::Intersection (int x = 0, int y = 0) : _x(x), _y(y) {}

Intersection Intersection::voisine (int indice) {
	if (indice == 0) return Intersection (_x - 1, _y);
	if (indice == 1) return Intersection (_x, _y - 1);
	if (indice == 2) return Intersection (_x + 1, _y);
	if (indice == 3) return Intersection (_x, _y + 1);
}


Intersection Intersection::diagonale(int indice) {
	if (indice == 0) return Intersection (_x - 1, _y - 1);
	if (indice == 1) return Intersection (_x + 1, _y - 1);
	if (indice == 2) return Intersection (_x + 1, _y + 1);
	if (indice == 3) return Intersection (_x - 1, _y + 1);
}