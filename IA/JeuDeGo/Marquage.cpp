#include "Marquage.h"

Marquage::Marquage() {
	_marqueur = 1;
	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < WIDTH; j++)
			_marquee [i] [j] = 0;
}

Marquage::Marquage(const Marquage& to_copy) {
	_marqueur = to_copy._marqueur;
	for(int i=0; i< WIDTH; ++i) {
		for(int j=0; j< WIDTH; ++j)
			_marquee[i][j]=to_copy._marquee[i][j];
	}
}

void Marquage::init() {
	_marqueur++;
}

void Marquage::marque(int i, int j) {
	_marquee [i] [j] = _marqueur;
}

void Marquage::marque(Intersection inter) {
	_marquee [inter.x_] [inter.y_] = _marqueur;
}