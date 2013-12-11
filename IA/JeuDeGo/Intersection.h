#ifndef INTERSECTION_H
#define INTERSECTION_H

class Intersection {
public :
	int x_, y_;

	Intersection(int x = 0, int y = 0);

	Intersection voisine(int indice);

	Intersection diagonale(int indice);
}; 

#endif 