#ifndef INTERSECTION_H
#define INTERSECTION_H

class Intersection {
public :
	char x_, y_;

	Intersection(char x = 0, char y = 0);

	Intersection voisine(int indice);

	Intersection diagonale(int indice);
}; 

#endif 