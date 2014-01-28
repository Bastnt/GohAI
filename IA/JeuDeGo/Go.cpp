#include "Go.h"

using namespace std;

Go::Go () : hash(0), nbCoupsJoues(0) {
	Mmc = 0;
	std::fill_n(Mbm, 5, 0);
	for(int i = 0; i < WIDTH; ++i) {
		vector<char> tmp;
		fill(tmp.begin(), tmp.begin()+WIDTH, 0);
		goban.push_back(tmp);
	}

	root_ = new Node();
	dejavu = new Marquage();
	dejavu2 = new Marquage();
}

Go::~Go () {
	delete root_;
	delete dejavu;
	delete dejavu2;
}

void Go::initHash () {
	for (int c = 0; c < 2; c++)
		for (int i = 0; i <= WIDTH; i++)
			for (int j = 0; j <= WIDTH; j++) {
				HashArray [c] [i] [j] = 0;
				for (int b = 0; b < 64; b++)
					if ((rand () / (RAND_MAX + 1.0)) > 0.5)
						HashArray [c] [i] [j] |= (1ull << b);
			}
	HashTurn = 0;
	for (int j = 0; j < 64; j++)
		if ((rand () / (RAND_MAX + 1.0)) > 0.5)
			HashTurn |= (1ull << j);
}

bool Go::coupLegal (Intersection inter, int couleur) {
	//(0;0) means skip a turn, always a legal move
	Intersection voisine;

	if (inter.x_ == -1)
		return true;

	if (goban [inter.x_][inter.x_] != Vide)
		return false;

	for (int i = 0; i < 4; i++) {
		voisine = inter.voisine (i);
		if (goban[inter.x_][inter.x_] == Vide)
			return true;
	}

	for (int i = 0; i < 4; i++) {
		voisine = inter.voisine (i);
		if (goban[inter.x_][inter.x_] == couleur)
			if (minLib (voisine, 2) > 1)
				return true;
	}

	unsigned long long h = hashSiJoue (inter, couleur);
	for (int i = nbCoupsJoues - 1; i >= 0; i--)
		if (HashHistory [i] == h)
			return false;

	return (minLibIfPlay (inter, couleur, 1) > 0);
}

unsigned long long Go::hashSiJoue (Intersection inter, int couleur) {
	unsigned long long h = hash;
	int adversaire = Case::Noir;
	if (couleur == Case::Noir)
		adversaire = Blanc;

	dejavu2->init ();
	dejavu2->marque (inter);
	h ^= HashArray [couleur] [inter.x_] [inter.y_];
	h ^= HashTurn;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		Intersection pierre;
		if (!dejavu2->marquee (voisine)) {
			if (goban[voisine.x_][voisine.x_] == adversaire)
				if (minLib (voisine, 2) == 1) {
					stack<Intersection> st;
					dejavu2->marque (voisine);
					st.push (voisine);
					while (!st.empty ()) {
						Intersection courante = st.top ();
						st.pop ();
						h ^= HashArray [goban[voisine.x_][voisine.x_]] [courante.x_] [courante.y_];
						for (int j = 0; j < 4; j++) {
							pierre = courante.voisine (j);
							if (goban[pierre.x_][pierre.x_] == adversaire)
								if (!dejavu2->marquee (pierre)) {
									dejavu2->marque (pierre);
									st.push (pierre);
								}
						}
					}
				}
		}
	}
	return h;
}

int Go::minLib (Intersection inter, int min) {
	stack<Intersection> st;
	int compteur = 0, couleur = goban[inter.x_][inter.x_];

	dejavu->init ();
	dejavu->marque (inter);
	st.push (inter);
	Intersection voisine;

	while (!st.empty ()) {
		Intersection inter = st.top ();
		st.pop ();
		for (int i = 0; i < 4; i++) {
			voisine = inter.voisine (i);
			if (!dejavu->marquee (voisine)) {
				dejavu->marque (voisine);
				if (goban[inter.x_][inter.x_] == Vide) {
					compteur++;
					if (compteur >= min)
						return compteur;
				}
				else if (goban[inter.x_][inter.x_] == couleur)
					st.push (voisine);
			}
		}
	}
	return compteur;
}

int Go::minLibIfPlay (Intersection intersection, int couleur, int min) {
	stack<Intersection> st;
	int compteur = 0;

	if (goban[intersection.x_][intersection.x_] == Vide) {
		dejavu2->init ();
		dejavu2->marque (intersection);
		st.push (intersection);
		while (!st.empty ()) {
			Intersection inter = st.top ();
			st.pop ();
			for (int i = 0; i < 4; i++) {
				Intersection voisine = inter.voisine (i);
				if (!dejavu2->marquee (voisine)) {
					dejavu2->marque (voisine);
					if (goban[intersection.x_][intersection.x_] == Vide) {
						compteur++;
						if (compteur >= min)
							return compteur;
					}
					if (goban[intersection.x_][intersection.x_] == couleur)
						st.push (voisine);
				}
			}
		}
		int adversaire = Noir;
		if (couleur == Noir)
			adversaire = Blanc;
		for (int i = 0; i < 4; i++) {
			Intersection voisine = intersection.voisine (i);
			if (goban[voisine.x_][voisine.x_] == adversaire)
				if (minLib (voisine, 2) == 1) {
					compteur++;
					if (compteur >= min)
						return compteur;
				}
		}
	}
	return compteur;
}

void Go::joue (Intersection inter, int couleur) {
	HashHistory [nbCoupsJoues] = hash;
	moves [nbCoupsJoues] = inter;
	hash ^= HashTurn;

	if (inter.x_ != -1) {
		posePierre (inter, couleur);
		enlevePrisonniers (inter, couleur);
	}

	++nbCoupsJoues;
}

void Go::posePierre (Intersection inter, int couleur) {
	goban[inter.x_][inter.x_] = couleur;
	hash ^= HashArray [couleur] [inter.x_] [inter.y_];
}

void Go::enlevePrisonniers (Intersection inter, int couleur) {
	stack<Intersection> st;
	Intersection voisine;
	int adversaire = Noir;
	if (couleur == Noir)
		adversaire = Blanc;
	for (int i = 0; i < 4; i++) {
		voisine = inter.voisine (i);
		if ((goban[voisine.x_][voisine.x_] == adversaire))
			if (minLib (voisine, 1) == 0)
				st.push (voisine);
	}
	while (!st.empty ()) {
		voisine = st.top ();
		st.pop ();
		if ((goban[voisine.x_][voisine.x_] == adversaire))
			enleveChaine (voisine);
	}
}

void Go::enleveChaine (Intersection intersection) {
	stack<Intersection> st;
	Intersection voisine;
	int couleur = goban[intersection.x_][intersection.y_];

	st.push (intersection);
	while (!st.empty ()) {
		Intersection inter = st.top ();
		st.pop ();
		hash ^= HashArray [couleur] [inter.x_] [inter.y_];
		goban[inter.x_][inter.y_] = Vide;
		for (int i = 0; i < 4; i++) {
			voisine = inter.voisine (i);
			if ((goban[inter.x_][inter.y_] == couleur))
				st.push (voisine);
		}
	}
}

bool Go::IsOut(Intersection i) {
	if(i.x_ > 0 && i.x_ < WIDTH && i.y_ > 0 && i.y_ < WIDTH)
		return true;
	return false;
}

bool Go::entouree (Intersection intersection, int couleur) {
	Intersection voisine;
	if (goban[intersection.x_][intersection.y_] != Vide)
		return false;
	for (int i = 0; i < 4; i++) {
		voisine = intersection.voisine (i);
		if(IsOut(voisine) || (goban[voisine.x_][voisine.y_] != couleur))
			return false;
	}
	for (int i = 0; i < 4; i++) {
		voisine = intersection.voisine (i);
		if ((goban[voisine.x_][voisine.y_] == couleur))
			if (minLib (voisine, 2) == 1)
				return false;
	}
	return true;
}

bool Go::protegee (Intersection inter, int couleur, bool & bord) {
	if (IsOut(inter)) {
		bord = true;
		return true;
	}
	if (goban[inter.x_][inter.y_] == Vide)
		if (entouree (inter, couleur))
			return true;
	if (goban[inter.x_][inter.y_] == couleur)
		return true;
	return false;
}

bool Go::oeil (Intersection inter, int couleur) {
	if (!entouree (inter, couleur))
		return false;

	Intersection diagonale;
	int nbDiagonalesProtegees = 0;
	bool bord = false;
	for (int i = 0; i < 4; i++) {
		diagonale = inter.diagonale (i);
		if (protegee (diagonale, couleur, bord))
			nbDiagonalesProtegees++;
	}

	if (bord && (nbDiagonalesProtegees == 4))
		return true;
	if (!bord && (nbDiagonalesProtegees > 2))
		return true;

	return false;
}

/* score a la chinoise : nb de pierres sur le goban */
void Go::calculeScores () {
	score [Noir] = 0;
	score [Blanc] = KOMI;
	Intersection inter;

	for (int i = 0; i <= WIDTH; i++)
		for (int j = 0; j <= WIDTH; j++)
			if (goban[i][j] == Vide) {
				inter.x_ = i;
				inter.y_ = j;
				if (entouree (inter, Noir))
					++score[Noir];
				else if (entouree (inter, Blanc))
					++score[Blanc];
			}
			else  
				++score[goban[i][j]];

	
	if (score [Blanc] >= score [Noir]) {
		score [Blanc] = 1;
		score [Noir] = 0;
	}
	else {
		score [Blanc] = 0;
		score [Noir] = 1;
	}
}

bool Go::gameOver () {
	if (moves[nbCoupsJoues - 1].x_ == -1 && moves[nbCoupsJoues - 2].x_ == -1)
		return true;
	return false;
}

Intersection Go::choisirUnCoup (int couleur) {
	int urgence [WIDTH] [WIDTH], nbUrgences = 0;
	Intersection inter;

	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < WIDTH; j++)
			if (goban[i][j] == Vide) {
				urgence[i][j] = 1;
				nbUrgences++;
			}
			else
				urgence[i][j] = 0;

	do {
		if (nbUrgences <= 0)
			return Intersection (-1, -1);
		int index = (int) (nbUrgences * (rand () / (RAND_MAX + 1.0)));
		int somme = 0;
		for (int i = 0; ((i <= WIDTH) && (somme <= index)); ++i)
			for (int j = 0; ((j <= WIDTH) && (somme <= index)); ++j)
				if (urgence[i][j] > 0) {
					somme += urgence[i][j];
					if (somme > index) {
						inter.x_ = i;
						inter.y_ = j;
					}
				}

				nbUrgences -= urgence[inter.x_][inter.y_];
				urgence[inter.x_][inter.y_] = 0;
	}
	while (entouree (inter, couleur) || 
		!coupLegal (inter, couleur));

	return inter;
}

void Go::playout (int color) {
	
	while(!gameOver()) {
		Intersection inter = choisirUnCoup(color);
		joue(inter, color);
		ChangeColor(color);
	}
	
	calculeScores();
}

void Go::ChangeColor(int& color) {
	color = color==Blanc ? Noir : Blanc;
}

// Useless
list<Intersection>& Go::GetLegalMoves(int color) {
	list<Intersection> moves;
	Intersection inter;
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			inter.x_=i;
			inter.y_=j;
			if(coupLegal(i, color)) {
				moves.push_back(i);
			}
		}
	}
	return moves;
}

Intersection Go::GetBestMove(long milliseconds, int color) {
	//Run the algorithm
	clock_t start_time=clock();
    while(clock() - start_time < milliseconds) {
		MontecarloAlgorithm(color);
	}

	if(root_->kodomo_.size() > 0) {
		cerr << "The root has no child, cannot find the best move amongst them!" << endl;
		return NULL;
	}

	//Retrieve the answer
	float score = (static_cast<float>(root_->kodomo_[0]->winCounter_));
	int best=0;
	for(int i=1, lg=root_->kodomo_.size(); i<lg; ++i) {
		if(score < static_cast<float>(root_->kodomo_[i]->winCounter_)) {
			best = i;
			score = static_cast<float>(root_->kodomo_[i]->winCounter_);
		}
	}
	cout << "The next move to perform is: (" << root_->moves_[best].x_ << ";" << root_->moves_[best].y_ << ")" << endl;
	return root_->moves_[best];
}

// Useless
void Go::UpdateGohanAndNode(Intersection move, int color) {
	joue(move, color);
	int i=0, lg=root_->moves_.size();
	while(i < lg) {
		if(move.x_==root_->moves_[i].x_ && move.y_==root_->moves_[i].y_)
			break;
		++i;
	}
	if(i != lg){
		//TODO: properly test the memory here
		root_ = root_->kodomo_[i];
		root_->becomeRoot();
	} else {
		cerr << "Could not find the proper child, try running montecarlo algorithm before using the method again" << endl;
	}


}

Node& Go::Select(Node& root, int& color) {
	clock_t start_time=clock();
	Node * best = &root;
	//Remember the position within the kodomo to update the goban with the good move 
	while(!best->isLeaf()) {
		int best_index;
		float max = -50.0f, score;
		for(int i=0, lg=best->kodomo_.size(); i<lg; ++i) {
			if(best->kodomo_[i]->playCounter_==0) {
				Mbm[simulate] += clock() - start_time;
				return *(best->kodomo_[i]);
			}
			else
			score = (static_cast<float>(best->kodomo_[i]->winCounter_) / best->kodomo_[i]->playCounter_)
					+ C * sqrt(log((float)root.playCounter_) / best->kodomo_[i]->playCounter_);
			if(score > max) {
				max = score;
				best_index=i;
			}
		}
		//update the gohan according to the color
		best = best->kodomo_[best_index];
		ChangeColor(color);
		joue(best->moves_[best_index], color);
	}
	Mbm[simulate] += clock() - start_time;
	return *best;
}

Node& Go::Expand(Node& node, int& color) {
	clock_t start_time=clock();
	ChangeColor(color);
	// Create the list of the children
	Intersection inter;
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			inter.x_ = i;
			inter.y_ = j;
			if(coupLegal(inter, color)) {
				node.moves_.push_back(inter);
				Node* n = new Node(&node);
				node.kodomo_.push_back(n);
			}
		}
	}
	Mbm[expand] += clock() - start_time;
	if(node.kodomo_.size() > 0)
		return *(node.kodomo_.front());
	cerr << "WARNING: Could not find any child, returned the node itself" << endl;
	return node;
}

void Go::Simulate(Node& node, int color) {
	clock_t start_time=clock();
	ChangeColor(color);
	SaveState();
	for(int i = 0; i < PLAYOUTS; ++i) {
		//We play the random playout
		playout(color);//start with a move of the color in parameter
		//The node has been played once more...
		++node.playCounter_;
		//.. and its winCounter has possibly evolved
		node.winCounter_ += score[color];
		//...and eventually restore the previous state
		RestoreState();
	}
	Mbm[simulate] += clock() - start_time;
}

void Go::BackPropage(Node& node) {
	clock_t start_time=clock();
	int play = node.playCounter_;
	int win = node.winCounter_;
	Node* current = &node;
	while(!current->isRoot()) {
		current = current->parent_;
		current->winCounter_ += win;
		current->playCounter_ += play;
	}
	Mbm[backpropage] += clock() - start_time;
}

void Go::MontecarloAlgorithm(int root_color) {
	++Mmc;
	/*
	Selection: starting from root R, select successive child nodes down to a leaf node L. The section below says more about a way of choosing child nodes that lets the game tree expand towards most promising moves, which is the essence of MCTS.
	Expansion: unless L ends the game, create none, one or more child nodes of it and choose from them node C. If none child was created, start simulation from L.
	Simulation: play a random playout from node C.
	Backpropagation: using the result of the playout, update information in the nodes on the path from C to R.
	*/

	//Creates a color (will be modified as a reference in the methods below)
	int color = root_color;

	//Selection
	Node& selected = Select(*root_, color);
	//Expansion
	Node& expanded = Expand(selected, color);
	//Simulation
	Simulate(expanded, color);
	//Backpropaging
	BackPropage(expanded);

}

void Go::DisplayGoban() {
	cout << "Goban:" << endl;
	for (int i = 0; i <= WIDTH; i++) {
		for (int j = 0; j <= WIDTH; j++) {
			switch(goban[i][j]) {
			case Vide: 
				cout << "V";
				break;
			case Blanc: 
				cout << "B";
				break;
			case Noir: 
				cout << "N";
				break;
			default:
				cout << "O";//other (=errror)
				break;
			}
		}
		cout << endl;
	}
}

void Go::SaveState() {
	save_goban = goban;
	save_nbCoupsJoues = nbCoupsJoues;
	save_hash = hash;
	//moves and HashHistory are saved due to nbCoupsJoues
	save_dejavu = new Marquage(*dejavu);
	save_dejavu2 = new Marquage(*dejavu2);
}

void Go::RestoreState(){
	goban = save_goban;
	nbCoupsJoues = save_nbCoupsJoues;
	hash = save_hash;
	dejavu = new Marquage(*save_dejavu);
	dejavu2 = new Marquage(*save_dejavu2);
}