#ifndef GOHAI
#define GOHAI

#include <vector>
#include <stack>
#include "Move.h"
#include "Node.h"

using namespace std;

//Bonus points for the player who does not start (7.5 in fact)
const char KOMI = 7;
// Number of random plays in simulate
const short PLAYOUTS = 5;
// Board size
const int WIDTH = 9;
//Used in the selection algorithm within the MCTS
const float C = 0.3;

const unsigned long long HASH_TURN = 9349610587725488279;

enum State { Noir = 0, Blanc = 1, Vide = 2 };

class GohAI {
public:
	// Board and game informations
	vector<vector<char>> goban_;
	vector<Move> moves_;
	int play_counter_;

	// MCTS
	Node *tree_search_;

	// Hash
	unsigned long long hash_array_[2][WIDTH][WIDTH];
	vector<unsigned long long> hash_history_;
	unsigned long long hash_;

	GohAI();
	~GohAI();
	void Init();
	bool isLegalMove(Move move, char color);
	int GetLiberties(Move move, int min_cutoff);
	void Play(Move move, char color);
	void Playout(char color);
	void PutStone(Move move, char color);
	void RemovePrisonners(Move move, char color);
	void RemoveChain(Move move);
	bool isSurrounded(Move move, char color);
	bool isProtected(Move move, char color);
	bool isEye(Move move, char color);
	bool isOut(Move move);
	bool GameOver();
	State GetWinner();
	Move NextMove(char color);
	Move NextMove2(char color);
	unsigned long long Hash(Move move, char color);

	//Changes the color in parameter from Blanc to Noir and Noir to Blanc
	inline void ChangeColor(char& color) { color = (color == Noir ? Blanc : Noir); }
	
	//Performs the selection within the MCTS, selecting the best Node at the moment, also updates the goban to be in the good state
	Node& Select(Node& root, char& color);
	//Performs the expansion within the MCTS, increases the number of considered Nodes
	Node& Expand(Node& node, char& color);
	//Performs the simulation within the MCTS, randomly plays until an end of game position
	void Simulate(Node& node, char color);
	//Performs the backpropagation within the MCTS, updating the parents Nodes
	void BackPropage(Node& node);
	//Does the main job
	void MontecarloAlgorithm(char color);

	Move GetBestMove(long milliseconds, char color);
};

#endif