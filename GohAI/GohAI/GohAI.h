#ifndef GOHAI
#define GOHAI

#include <iostream>
#include <vector>
#include <stack>
#include "Move.h"
#include "Node.h"


using namespace std;

//Bonus points for the player who does not start (7.5 in fact)
const char KOMI = 7;
//Number of random plays in simulate
const short PLAYOUTS = 5;
//Board size
const int WIDTH = 9;
//Used in the selection algorithm within the MCTS
const float C = 0.3;

//The unique value representing a turn shift
const unsigned long long HASH_TURN = 9349610587725488279;

//The state of a cell, black (Noir), white (Blanc), empty (Vide)
enum State { Noir = 0, Blanc = 1, Vide = 2 };

/*
	Artificial Intelligence for the go
*/
class GohAI {
public:
	// Board and game informations
	vector<vector<char>> goban_;
	//The moves performed from the beginning
	vector<Move> moves_;

	static const Move STARTING_MOVES[25];

	//The tree
	Node *tree_search_;

	//Hash values for all <move,color>, randomly chosen
	unsigned long long hash_array_[2][WIDTH][WIDTH];
	//The history of the previous hashes
	vector<unsigned long long> hash_history_;
	//The actual hash value, holds a "state" of the game
	unsigned long long hash_;

	GohAI();
	~GohAI();
	//Initializes the hash_array attribute with random values
	void Init();
	//Checks whether a provided move of the provided color is legal or not
	bool isLegalMove(Move move, char color);
	/*Gets the number of liberties of a position, the min_cutoff is used for reduce the execution 
	time of the method when a specified value is "enough" as a number of liberties*/
	int GetLiberties(Move move, int min_cutoff);
	//Plays the move of the color, updates hash and gohan (amongst other stuff)
	void Play(Move move, char color);
	//Randomly plays the move of the color, updates hash and gohan (amongst other stuff)
	void Playout(char color);
	//Puts a stone of the provided color at the specified position (do not remove prisoners)
	void PutStone(Move move, char color);
	//Removes the prisoners from the goban
	void RemovePrisoners(Move move, char color);
	//Removes linked stones from the goban
	void RemoveChain(Move move);
	//Checks if a stone is completed surrounded
	bool isSurrounded(Move move, char color);
	//Checks if a stone is protected
	bool isProtected(Move move, char color);
	//Checks if there is an eye on this position
	bool isEye(Move move, char color);
	//Checks if the move is out of the gohan
	bool isOut(Move move);
	//Checks if the game is over
	bool GameOver();
	//Returns the winner of the game
	State GetWinner();
	//Finds a move to perform in playout
	Move NextMove(char color);
	//Finds a move to perform in playout (second implementation)
	Move NextMove2(char color);
	//Returns the hash after performing a specific move
	unsigned long long Hash(Move move, char color);
	// Rellocates the root of the tree_search_
	void ReallocateRoot(Move move);


	void DisplayGoban();

	//Changes the color in parameter from Blanc to Noir and Noir to Blanc
	inline void ChangeColor(char& color) { color = (color == Noir ? Blanc : Noir); }
	
	/*Performs the selection within the MCTS, selecting the best Node at the moment, 
	also updates the goban to be in the good state*/
	Node& Select(Node& root, char& color);
	//Performs the expansion within the MCTS, increases the number of considered Nodes
	Node& Expand(Node& node, char& color);
	//Performs the simulation within the MCTS, randomly plays until an end of game position
	void Simulate(Node& node, char color);
	//Performs the backpropagation within the MCTS, updates the parents Nodes
	void BackPropage(Node& node);
	//Does the main job
	void MontecarloAlgorithm(char color);
	//Returns the best move to perform, for the provided color, after running the algorithm the provided amount of time
	Move GetBestMove(long milliseconds, char color);
};

#endif