#include <math.h>
#include <iostream>
#include <stack>
#include <list>
#include <limits>

#include "Intersection.h"
#include "Marquage.h"
#include "Node.h"

#ifndef GO_H
#define GO_H

const float KOMI = 7.5;
const int C = 0.3;

enum Case { Noir = 0, Blanc = 1, Vide = 2, Exterieur = 3 };

const int MaxCoups = 300;
Marquage dejavu, dejavu2;

unsigned long long HashArray [2] [Taille + 2] [Taille + 2];
unsigned long long HashTurn;

class Go {
public:
	char goban [Taille + 2] [Taille + 2];
	int nbCoupsJoues;
	Intersection moves [MaxCoups];
	unsigned long long hash;
	unsigned long long HashHistory [MaxCoups];
	float score [2];

	const static float C;
	Node root;

	Go();
	void initHash();
	bool coupLegal(Intersection inter, int couleur);
	unsigned long long hashSiJoue(Intersection inter, int couleur);
	int minLib(Intersection inter, int min);
	int minLibIfPlay(Intersection intersection, int couleur, int min);
	void joue (Intersection inter, int couleur);
	void posePierre (Intersection inter, int couleur);
	void enlevePrisonniers (Intersection inter, int couleur);
	void enleveChaine (Intersection intersection);
	bool entouree (Intersection intersection, int couleur);
	bool protegee (Intersection inter, int couleur, bool & bord);
	bool oeil (Intersection inter, int couleur);
	void calculeScores ();
	bool gameOver ();
	Intersection choisirUnCoup (int couleur);
	void playout (int couleur);

	list<Intersection>& Go::GetLegalMoves(int color);
	void montecarloAlgorithm (int color);
	Node& Select(Node& explored);
	void Expand(Node& node, int couleur);
	void Simulate(Node& node, int color);
};

#endif