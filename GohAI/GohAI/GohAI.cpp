#include "GohAI.h"

GohAI::GohAI() : hash_(0ull) {
	//srand(time(NULL));
	for(int i = 0; i < WIDTH; ++i) {
		std::vector<char> tmp (WIDTH,Vide);
		goban_.push_back(tmp);
	}

	tree_search_ = new Node(NULL, Move());
	Node* tmp;
	for(int i = 0; i < WIDTH; ++i)
		for(int j = 0; j < WIDTH; ++j) {
			tmp = new Node(tree_search_, Move(i,j));
			
			if(i < 2 || i > 6 || j < 2 || j > 6)
				tmp->playCounter_ = PLAYOUTS;

			tree_search_->children_.push_back(tmp);
		}
}

GohAI::~GohAI () {
	delete tree_search_;
}

void GohAI::Init() {
	for (int c = 0; c < 2; ++c)
		for (int i = 0; i < WIDTH; ++i)
			for (int j = 0; j < WIDTH; ++j) {
				hash_array_[c][i][j] = 0;
				for (int b = 0; b < 64; ++b)
					if ((rand () / (RAND_MAX + 1.0)) > 0.5)
						hash_array_[c][i][j] |= (1ull << b);
			}
}

bool GohAI::isLegalMove(Move move, char color) {
	Move neighbor;

	if (move.x_ == -1)
		return true;

	if (goban_[move.x_][move.y_] != Vide)
		return false;

	for (int i = 0; i < 4; ++i) {
		neighbor = Move::GetNeighbour(move, i);
		if(isOut(neighbor))
			continue;
		if (goban_[neighbor.x_][neighbor.y_] == Vide)
			return true;
	}

	for (int i = 0; i < 4; i++) {
		neighbor = Move::GetNeighbour(move, i);
		if(isOut(neighbor))
			continue;
		if (goban_[neighbor.x_][neighbor.y_] == color)
			if (GetLiberties(neighbor, 2) > 1)
				return true;
	}

	unsigned long long h = Hash(move, color);
	//We start from the end because it is more likely to occur
	for (auto it = hash_history_.rbegin(); it != hash_history_.rend(); ++it)
		if (*it == h)
			return false;

	char old = goban_[move.x_][move.y_];
	goban_[move.x_][move.y_] = color;
	bool ret = GetLiberties(move, 1);
	goban_[move.x_][move.y_] = old;
	return ret;
}

unsigned long long GohAI::Hash(Move move, char color) {
	unsigned long long h = hash_;
	int opponent = color == Noir ? Noir : Blanc;

	static bool remember[WIDTH][WIDTH] = {0};

	remember[move.x_][move.y_] = 1;
	h ^= hash_array_[color][move.x_][move.y_];
	h ^= HASH_TURN;
	Move neighbor, rock, position;
	for (int i = 0; i < 4; ++i) {
		neighbor = Move::GetNeighbour(move, i);
		if(isOut(neighbor))
			continue;
		if (!remember[neighbor.x_][neighbor.y_]) {
			if (goban_[neighbor.x_][neighbor.y_] == opponent)
				if (GetLiberties(neighbor, 2) == 1) {
					vector<Move> queue;
					remember[neighbor.x_][neighbor.y_] = 1;
					queue.push_back (neighbor);

					while (!queue.empty ()) {
						position = queue.back();
						queue.pop_back();
						h ^= hash_array_[goban_[neighbor.x_][neighbor.y_]] [position.x_][position.y_];
						for (int j = 0; j < 4; ++j) {
							rock = Move::GetNeighbour(position, j);
							if(isOut(rock))
								continue;
							if (goban_[rock.x_][rock.y_] == opponent)
								if (!remember[rock.x_][rock.y_]) {
									remember[rock.x_][rock.y_] = 1;
									queue.push_back(rock);
								}
						}
					}
				}
		}
	}
	return h;
}

int GohAI::GetLiberties(Move move, int min_cutoff) {
	static vector<Move> queue;
	queue.clear();
	int compteur = 0, color = goban_[move.x_][move.y_];

	bool remember[WIDTH][WIDTH] = {0};
	remember[move.x_][move.y_] = 1;
	queue.push_back(move);
	Move neighbor, position;

	while (!queue.empty()) {
		position = queue.back ();
		queue.pop_back();
		for (int i = 0; i < 4; ++i) {
			neighbor = Move::GetNeighbour(position, i);
			if(isOut(neighbor))
				continue;
			if (!remember[neighbor.x_][neighbor.y_]) {
				remember[neighbor.x_][neighbor.y_] = 1;
				if (goban_[neighbor.x_][neighbor.y_] == Vide) {
					++compteur;
					if (compteur >= min_cutoff)
						return compteur;
				}
				else if (goban_[neighbor.x_][neighbor.y_] == color)
					queue.push_back(neighbor);
			}
		}
	}
	return compteur;
}

void GohAI::Play(Move move, char color) {
	hash_history_.push_back(hash_);
	moves_.push_back(move);
	hash_ ^= HASH_TURN;

	// If we do not pass our turn
	if (move.x_ != -1) {
		PutStone(move, color);
		RemovePrisoners(move, color);
	}
}

void GohAI::Playout(char color) {
	Move move;
	while(!GameOver()) {
		move = NextMove(color);
		Play(move, color);
		ChangeColor(color);
	}
}

void GohAI::PutStone(Move move, char color) {
	goban_[move.x_][move.y_] = color;
	hash_ ^= hash_array_[color][move.x_][move.y_];
}

void GohAI::RemovePrisoners(Move move, char color) {
	vector<Move> queue;
	Move neighbor;
	int opponent = (color == Noir ? Blanc : Noir);

	for (int i = 0; i < 4; ++i) {
		neighbor = Move::GetNeighbour(move, i);
		if(isOut(neighbor))
			continue;
		if ((goban_[neighbor.x_][neighbor.y_] == opponent))
			if (GetLiberties(neighbor, 1) == 0)
				queue.push_back(neighbor);
	}
	while (!queue.empty ()) {
		neighbor = queue.back();
		queue.pop_back();
		if ((goban_[neighbor.x_][neighbor.y_] == opponent))
			RemoveChain(neighbor);
	}
}

void GohAI::RemoveChain(Move move) {
	vector<Move> queue;
	Move neighbor, position;
	int color = goban_[move.x_][move.y_];

	queue.push_back(move);
	while (!queue.empty()) {
		position = queue.back();
		queue.pop_back();
		hash_ ^= hash_array_[color][position.x_][position.y_];
		goban_[position.x_][position.y_] = Vide;
		for (int i = 0; i < 4; ++i) {
			neighbor = Move::GetNeighbour(position, i);
			if(isOut(neighbor))
				continue;
			if ((goban_[neighbor.x_][neighbor.y_] == color))
				queue.push_back(neighbor);
		}
	}
}

bool GohAI::isSurrounded(Move move, char color) {
	Move neighbor;
	if (goban_[move.x_][move.y_] != Vide)
		return false;
	for (int i = 0; i < 4; ++i) {
		neighbor = Move::GetNeighbour(move, i);
		if(isOut(neighbor))
			return false;
		if(goban_[neighbor.x_][neighbor.y_] != color)
			return false;
	}
	for (int i = 0; i < 4; ++i) {
		neighbor = Move::GetNeighbour(move, i);
		if (GetLiberties(neighbor, 2) == 1)
			return false;
	}
	return true;
}

bool GohAI::isProtected(Move move, char color) {
	if (isOut(move))
		return true;

	if (goban_[move.x_][move.y_] == Vide)
		if (isSurrounded(move, color))
			return true;
	if (goban_[move.x_][move.y_] == color)
		return true;
	return false;
}

bool GohAI::isEye(Move move, char color) {

	bool out = isOut(move);

	if (!isSurrounded(move, color))
		return false;

	Move diagonale;
	int protected_diagonales = 0;
	for (int i = 0; i < 4; ++i) {
		diagonale = Move::GetDiagonal(move, i);
		if (isProtected(diagonale, color))
			++protected_diagonales;
	}

	if (out && (protected_diagonales == 4))
		return true;
	if (!out && (protected_diagonales > 2))
		return true;

	return false;
}

bool GohAI::isOut(Move move) {
	if(move.x_ < 0 || move.x_ >= WIDTH || move.y_ < 0 || move.y_ >= WIDTH)
		return true;
	return false;
}

bool GohAI::GameOver() {
	auto it = moves_.end();
	if (moves_.size() > 1 && (it-1)->x_ == -1 && (it-2)->x_ == -1)
		return true;
	return false;
}

State GohAI::GetWinner() {
	unsigned char score[2] = {0};
	score [Blanc] = KOMI;
	Move move;

	for (int i = 0; i < WIDTH; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (goban_[i][j] == Vide) {
				move.x_ = i;
				move.y_ = j;
				if (isSurrounded(move, Noir))
					++score[Noir];
				else if (isSurrounded(move, Blanc))
					++score[Blanc];
			}
			else  
				++score[goban_[i][j]];

	if (score [Blanc] >= score [Noir]) {
		return Blanc;
	}
	else {
		return Noir;
	}
}

Move GohAI::NextMove2(char color) {
	int urgence[WIDTH][WIDTH], nbUrgences = 0;
	Move move;

	for (int i = 0; i < WIDTH; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (goban_[i][j] == Vide) {
				urgence[i][j] = 1;
				++nbUrgences;
			}
			else
				urgence[i][j] = 0;

	do {
		if (nbUrgences <= 0)
			return Move(-1, -1);
		int index = (int) (nbUrgences * (rand() / (RAND_MAX + 1.0)));
		int somme = 0;
		for (int i = 0; ((i < WIDTH) && (somme <= index)); ++i)
			for (int j = 0; ((j < WIDTH) && (somme <= index)); ++j)
				if (urgence[i][j] > 0) {
					somme += urgence[i][j];
					if (somme > index) {
						move.x_ = i;
						move.y_ = j;
					}
				}
		nbUrgences -= urgence[move.x_][move.y_];
		urgence[move.x_][move.y_] = 0;
	}
	while (isSurrounded(move, color) || 
		!isLegalMove(move, color));

	return move;
}

Move GohAI::NextMove(char color) {
	Move move;

	unsigned char nbUrgences = 0;
	bool urgence[WIDTH][WIDTH];

	for (int i = 0; i < WIDTH; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (goban_[i][j] == Vide) {
				urgence[i][j] = true;
				++nbUrgences;
			}
			else
				urgence[i][j] = false;

	do {
		if (nbUrgences == 0)
			return Move(-1, -1);
		int index = (int) (nbUrgences * (rand() / (RAND_MAX + 1.0)));
		int somme = 0;
		for (int i = 0; ((i < WIDTH) && (somme <= index)); ++i)
			for (int j = 0; j < WIDTH; ++j)
				if (urgence[i][j]) {
					++somme;
					if (somme > index) {
						move.x_ = i;
						move.y_ = j;
						break;
					}
				}
		--nbUrgences;
		urgence[move.x_][move.y_] = 0;
	}
	while (isSurrounded(move, color) || 
		!isLegalMove(move, color));

	return move;
}

Move GohAI::GetBestMove(long milliseconds, char color) {
	//Run the algorithm
	clock_t start_time=clock();
    while(clock() - start_time < milliseconds) {
		MontecarloAlgorithm(color);
	}

	if(tree_search_->children_.size() == 0) {
		return NULL;
	}

	//Retrieve the answer
	float score = 0, candidate;
	Node* best_node;
	for(auto it = tree_search_->children_.begin(); it != tree_search_->children_.end(); ++it) {
		if((*it)->playCounter_ == 0)
			continue;
		candidate = static_cast<float>((*it)->winCounter_) / (*it)->playCounter_;
		if(score < candidate) {
			best_node = *it;
			score = candidate;
		}
	}

 	return best_node->move_;
}

void GohAI::MontecarloAlgorithm(char root_color) {
	char color = root_color;

	auto backup_hash_history_ = hash_history_;
	auto backup_hash_ = hash_;
	auto backup_moves_ = moves_;
	auto backup_goban_ = goban_;

	//Selection
	Node* selected = Select(tree_search_, color);
	//Expansion
	selected = Expand(selected, color);
	//Simulation
	Simulate(selected, color, root_color);
	//Backpropaging
	BackPropage(selected);

	// Restore old values
	goban_ = backup_goban_;
	moves_ = backup_moves_;
	hash_ = backup_hash_;
	hash_history_ = backup_hash_history_;
}

Node* GohAI::Select(Node* root, char& color) {
	Node * best = root;
	//Remember the position within the kodomo to update the goban with the good move 
	while(!best->isLeaf()) {
		for(auto it = best->children_.begin(); it != best->children_.end(); ++it) {
			if((*it)->playCounter_ == 0)
				return *it;
		}

		Node* best_node;
		float max = -500.0f, score;
		for(auto it = best->children_.begin(); it != best->children_.end(); ++it) {
			score = (static_cast<float>((*it)->winCounter_) / (*it)->playCounter_)
					+ C * sqrt(log((float)best->playCounter_) / (*it)->playCounter_);

			if(score > max) {
				max = score;
				best_node=*it;
			}
		}
		//update the gohan according to the color
		best = best_node;
		ChangeColor(color);
		Play(best->move_, color);
	}
	return best;
}

Node* GohAI::Expand(Node* node, char& color) {
	ChangeColor(color);
	// Create the list of the children
	Move inter;
	Node* n;
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			inter.x_ = i;
			inter.y_ = j;
			if(isLegalMove(inter, color) && !isSurrounded(inter, color)) {
				n = new Node(node, inter);
				if(i < 2 || i > 6 || j < 2 || j > 6)
					n->playCounter_ = -PLAYOUTS;

				node->children_.push_back(n);
			}
		}
	}

	if(node->children_.size() > 0)
		return node->children_.front();
	return node;
}

void GohAI::Simulate(Node* node, char starting_color, char player) {
	ChangeColor(starting_color);

	// Save values tu replay from old state
	auto backup_hash_history_ = hash_history_;
	auto backup_hash_ = hash_;
	auto backup_moves_ = moves_;
	auto backup_goban_ = goban_;

	for(int i = 0; i < PLAYOUTS; ++i) {
		//We play the random playout
		Playout(starting_color); //start with a move of the color in parameter
		//The node has been played once more...
		++node->playCounter_;
		//.. and its winCounter has possibly evolved
		if(GetWinner() == player)
			++node->winCounter_;
		//...and eventually restore the previous state
		
		// Restore old values
		goban_ = backup_goban_;
		moves_ = backup_moves_;
		hash_ = backup_hash_;
		hash_history_ = backup_hash_history_;
	}
}

void GohAI::BackPropage(Node* node) {
	int play = node->playCounter_;
	int win = node->winCounter_;
	Node* current = node;
	while(!current->isRoot()) {
		current = current->parent_;
		current->winCounter_ += win;
		current->playCounter_ += play;
	}
}

void GohAI::ReallocateRoot(Move move) {
	// TODO free memory
	for(auto it = tree_search_->children_.begin(); it != tree_search_->children_.end(); ++it) {
		if((*it)->move_ == move) {
			tree_search_ = *it;
			tree_search_->parent_ = NULL;
			break;
		}
	}
}

void GohAI::DisplayGoban() {
	std::cout << "Goban:\n";
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			switch(goban_[i][j]) {
			case Vide: 
				std::cout << "-";
				break;
			case Blanc: 
				std::cout << "B";
				break;
			case Noir: 
				std::cout << "N";
				break;
			}
		}
		std::cout << "\n";
	}
}