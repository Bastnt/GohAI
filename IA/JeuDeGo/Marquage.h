#ifndef MARQUAGE_H
#define MARQUAGE_H

#include "Intersection.h"
#include "Go.h"

class Marquage {
	unsigned long long _marqueur;
	unsigned long long _marquee [WIDTH] [WIDTH];

public:
	Marquage ();

	Marquage(const Marquage& to_copy);

	void init ();

	inline bool marquee (int i, int j);
	void marque (int i, int j);

	inline bool marquee (Intersection inter);
	void marque (Intersection inter);
};

bool Marquage::marquee (int i, int j) {
	return (_marquee [i] [j] == _marqueur);
}

bool Marquage::marquee (Intersection inter) {
	return (_marquee [inter.x_] [inter.y_] == _marqueur);
}

#endif 