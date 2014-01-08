#ifndef GO_H
#define GO_H
#include <math.h>
#include <iostream>
#include <stack>
#include <list>
#include <limits>
#include <time.h>

#include "Intersection.h"
#include "Marquage.h"
#include "Node.h"
//Bonus points for the player who does not start
const float KOMI = 7.5;

const int WIDTH = Taille + 2;

const short PLAYOUTS = 10;

//Used in the selection algorithm within the MCTS
const int C = 0.3;

enum Case { Noir = 0, Blanc = 1, Vide = 2, Exterieur = 3 };

const int MaxCoups = 300;


class Go {
public:
	Marquage dejavu, dejavu2;
	unsigned long long HashArray [2] [WIDTH] [WIDTH];
	unsigned long long HashTurn;
	char* goban;
	int nbCoupsJoues;
	Intersection moves [MaxCoups];
	unsigned long long hash;
	unsigned long long HashHistory [MaxCoups];
	float score [2];

	const static float C;
	//Represents the current knowledge of the montecarlo algorithm on the goban
	Node* root_;

	Go();
	~Go();
	int c(int i, int j);
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
	//Allow us to calculate the score of a goban
	void calculeScores ();
	bool gameOver ();
	Intersection choisirUnCoup (int couleur);
	void playout (int couleur);

	//Copies the goban so that it can be restored afterwards
	char* CopyGoban(char* src);
	//Get tge list of legal moves for a color within the actual goban
	list<Intersection>& Go::GetLegalMoves(int color);
	//Performs the selection within the MCTS, selecting the best Node at the moment, also update the goban to be in the good state
	Node& Select(Node& explored);
	//Performs the expansion within the MCTS, increasing the number of considered Nodes
	Node& Expand(Node& node, int couleur);
	//Performs the simulation within the MCTS, randomly playing until an end game position
	void Simulate(Node& node, int color);
	//Performs the backpropagation within the MCTS, updating the parents Nodes
	void BackPropage(Node& node);
	//Does the main job
	void MontecarloAlgorithm (int color);
	//Update the gohan and the root node, allowing us to conserve our previous results for more accuracy
	void UpdateGohanAndNode(Intersection move, int color);
	//Displays the goban in the console, helping debug
	void DisplayGoban();

	//The best move to perform after processing MCTS for the provided amount of time
	Intersection GetBestMove(long seconds, int color);
	
};

#endif