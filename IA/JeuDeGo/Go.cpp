#include "Go.h"

using namespace std;

const float Go::C = 0.3;

Go::Go () {
	hash = 0;
	nbCoupsJoues = 0;
	goban = new char[WIDTH*WIDTH];
	for (int i = 1; i <= Taille; i++)
		for (int j = 1; j <= Taille; j++)
			goban [c(i,j)] = Vide;
	for (int i = 0; i < WIDTH; i++) {
		goban [c(0,i)] = Exterieur;
		goban [c(i,0)] = Exterieur;
		goban [c(WIDTH-1,i)] = Exterieur;
		goban [c(i,WIDTH-1)] = Exterieur;
	}
	root_ = new Node();
}

Go::~Go () {
	delete[] goban;
}

int Go::c(int i, int j) {
	return i + j * WIDTH;
}

void Go::initHash () {
	for (int c = 0; c < 2; c++)
		for (int i = 1; i <= Taille; i++)
			for (int j = 1; j <= Taille; j++) {
				HashArray [c] [i] [j] = 0;
				for (int b = 0; b < 64; b++)
					if ((rand () / (RAND_MAX + 1.0)) > 0.5)
						HashArray [c] [i] [j] |= (1ULL << b);
			}
			HashTurn = 0;
			for (int j = 0; j < 64; j++)
				if ((rand () / (RAND_MAX + 1.0)) > 0.5)
					HashTurn |= (1ULL << j);
}

bool Go::coupLegal (Intersection inter, int couleur) {
	if ((inter.x_ == 0) && (inter.y_ == 0))
		return true;

	if (goban [c(inter.x_, inter.y_)] != Vide)
		return false;

	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if (goban[c(voisine.x_, voisine.y_)] == Vide)
			return true;
	}

	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if (goban[c(voisine.x_, voisine.y_)] == couleur)
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

	dejavu2.init ();
	dejavu2.marque (inter);
	h ^= HashArray [couleur] [inter.x_] [inter.y_];
	h ^= HashTurn;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if (!dejavu2.marquee (voisine)) {
			if (goban[c(voisine.x_,voisine.y_)] == adversaire)
				if (minLib (voisine, 2) == 1) {
					stack<Intersection> st;
					dejavu2.marque (voisine);
					st.push (voisine);
					while (!st.empty ()) {
						Intersection courante = st.top ();
						st.pop ();
						h ^= HashArray [goban[c(voisine.x_,voisine.y_)]] [courante.x_] [courante.y_];
						for (int j = 0; j < 4; j++) {
							Intersection pierre = courante.voisine (j);
							if (goban[c(pierre.x_,pierre.y_)] == adversaire)
								if (!dejavu2.marquee (pierre)) {
									dejavu2.marque (pierre);
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
	int compteur = 0, couleur = goban[c(inter.x_,inter.y_)];

	dejavu.init ();
	dejavu.marque (inter);
	st.push (inter);
	while (!st.empty ()) {
		Intersection inter = st.top ();
		st.pop ();
		for (int i = 0; i < 4; i++) {
			Intersection voisine = inter.voisine (i);
			if (!dejavu.marquee (voisine)) {
				dejavu.marque (voisine);
				if (goban[c(voisine.x_,voisine.y_)] == Vide) {
					compteur++;
					if (compteur >= min)
						return compteur;
				}
				else if (goban[c(voisine.x_,voisine.y_)] == couleur)
					st.push (voisine);
			}
		}
	}
	return compteur;
}

int Go::minLibIfPlay (Intersection intersection, int couleur, int min) {
	stack<Intersection> st;
	int compteur = 0;

	if (goban[c(intersection.x_,intersection.y_)] == Vide) {
		dejavu2.init ();
		dejavu2.marque (intersection);
		st.push (intersection);
		while (!st.empty ()) {
			Intersection inter = st.top ();
			st.pop ();
			for (int i = 0; i < 4; i++) {
				Intersection voisine = inter.voisine (i);
				if (!dejavu2.marquee (voisine)) {
					dejavu2.marque (voisine);
					if (goban[c(voisine.x_,voisine.y_)] == Vide) {
						compteur++;
						if (compteur >= min)
							return compteur;
					}
					if (goban[c(voisine.x_,voisine.y_)] == couleur)
						st.push (voisine);
				}
			}
		}
		int adversaire = Noir;
		if (couleur == Noir)
			adversaire = Blanc;
		for (int i = 0; i < 4; i++) {
			Intersection voisine = intersection.voisine (i);
			if (goban[c(voisine.x_,voisine.y_)] == adversaire)
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

	if (inter.x_ != 0) {
		posePierre (inter, couleur);
		enlevePrisonniers (inter, couleur);
	}

	nbCoupsJoues++;
}

void Go::posePierre (Intersection inter, int couleur) {
	goban[c(inter.x_,inter.y_)] = couleur;
	hash ^= HashArray [couleur] [inter.x_] [inter.y_];
}

void Go::enlevePrisonniers (Intersection inter, int couleur) {
	stack<Intersection> st;
	int adversaire = Noir;
	if (couleur == Noir)
		adversaire = Blanc;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if ((goban[c(voisine.x_,voisine.y_)] == adversaire))
			if (minLib (voisine, 1) == 0)
				st.push (voisine);
	}
	while (!st.empty ()) {
		Intersection voisine = st.top ();
		st.pop ();
		if ((goban[c(voisine.x_,voisine.y_)] == adversaire))
			enleveChaine (voisine);
	}
}

void Go::enleveChaine (Intersection intersection) {
	stack<Intersection> st;
	int couleur = goban[c(intersection.x_,intersection.y_)];

	st.push (intersection);
	while (!st.empty ()) {
		Intersection inter = st.top ();
		st.pop ();
		hash ^= HashArray [couleur] [inter.x_] [inter.y_];
		goban[c(inter.x_,inter.y_)] = Vide;
		for (int i = 0; i < 4; i++) {
			Intersection voisine = inter.voisine (i);
			if ((goban[c(voisine.x_,voisine.y_)] == couleur))
				st.push (voisine);
		}
	}
}

bool Go::entouree (Intersection intersection, int couleur) {
	if (goban[c(intersection.x_,intersection.y_)] != Vide)
		return false;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = intersection.voisine (i);
		if ((goban[c(voisine.x_,voisine.y_)] != couleur) &&
			(goban[c(voisine.x_,voisine.y_)] != Exterieur))
			return false;
	}
	for (int i = 0; i < 4; i++) {
		Intersection voisine = intersection.voisine (i);
		if ((goban[c(voisine.x_,voisine.y_)] == couleur))
			if (minLib (voisine, 2) == 1)
				return false;
	}
	return true;
}

bool Go::protegee (Intersection inter, int couleur, bool & bord) {
	if (goban[c(inter.x_,inter.y_)] == Exterieur) {
		bord = true;
		return true;
	}
	if (goban[c(inter.x_,inter.y_)] == Vide)
		if (entouree (inter, couleur))
			return true;
	if (goban[c(inter.x_,inter.y_)] == couleur)
		return true;
	return false;
}

bool Go::oeil (Intersection inter, int couleur) {
	if (!entouree (inter, couleur))
		return false;

	int nbDiagonalesProtegees = 0;
	bool bord = false;
	for (int i = 0; i < 4; i++) {
		Intersection diagonale = inter.diagonale (i);
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

	for (int i = 1; i <= Taille; i++)
		for (int j = 1; j <= Taille; j++)
			if (goban[c(i,j)] == Vide) {
				Intersection inter (i, j);
				if (entouree (inter, Noir))
					score [Noir] += 1.0;
				else if (entouree (inter, Blanc))
					score [Blanc] += 1.0;
			}
			else  
				score [goban[c(i,j)]] += 1.0;

	if (true) {
		if (score [Blanc] > score [Noir]) {
			score [Blanc] = 1.0;
			score [Noir] = 0.0;
		}
		else {
			score [Blanc] = 0.0;
			score [Noir] = 1.0;
		}
	}
}

bool Go::gameOver () {
	int last = nbCoupsJoues - 1, nbPass = 0;
	while ((last > 0) && (moves [last].x_ == 0)) {
		last--;
		nbPass++;
	}
	if (nbPass >= 2)    
		return true;
	return false;
}

Intersection Go::choisirUnCoup (int couleur) {
	int urgence [Taille + 2] [Taille + 2], nbUrgences = 0;
	Intersection inter;

	for (int i = 1; i <= Taille; i++)
		for (int j = 1; j <= Taille; j++)
			if (goban[c(i,j)] == Vide) {
				urgence [i] [j] = 1;
				nbUrgences++;
			}
			else
				urgence [i] [j] = 0;

	do {
		if (nbUrgences <= 0)
			return Intersection (0, 0);
		int index = (int) (nbUrgences * (rand () / (RAND_MAX + 1.0)));
		int somme = 0;
		for (int i = 1; ((i <= Taille) && (somme <= index)); i++)
			for (int j = 1; ((j <= Taille) && (somme <= index)); j++)
				if (urgence [i] [j] > 0) {
					somme += urgence [i] [j];
					if (somme > index) {
						inter = Intersection (i, j);
					}
				}

				nbUrgences -= urgence [inter.x_] [inter.y_];
				urgence [inter.x_] [inter.y_] = 0;
	}
	while (entouree (inter, couleur) || 
		!coupLegal (inter, couleur));

	return inter;
}

void Go::playout (int couleur) {
	while(true) {
		if ((nbCoupsJoues >= MaxCoups) || gameOver ())
			break;

		Intersection inter = choisirUnCoup (couleur);

		joue (inter, couleur);
		if (couleur == Noir)
			couleur = Blanc;
		else
			couleur = Noir;
	}
	calculeScores ();
}

list<Intersection>& Go::GetLegalMoves(int color) {
	list<Intersection> moves;
	for (int i = 0; i < Taille; ++i) {
		for (int j = 0; j < Taille; ++j) {
			Intersection i(i,j);
			if(coupLegal(i, color)) {
				moves.push_back(i);
			}
		}
	}
	return moves;
}

char* Go::CopyGoban(char* src) {
	char dest[WIDTH*WIDTH];
	for(int i = 0; i < WIDTH*WIDTH; ++i) {
		dest[i] = src[i];
	}
	return dest;
}

void Go::MontecarloAlgorithm (int color) {
	Node selected = root_;
	/*
	Selection: starting from root R, select successive child nodes down to a leaf node L. The section below says more about a way of choosing child nodes that lets the game tree expand towards most promising moves, which is the essence of MCTS.
	Expansion: unless L ends the game, create none, one or more child nodes of it and choose from them node C. If none child was created, start simulation from L.
	Simulation: play a random playout from node C.
	Backpropagation:using the result of the playout, update information in the nodes on the path from C to R.
	*/

	// Selection
	Select(selected);
		
	//Expansion
	selected = Expand(selected, color);

	//Simulation
	Simulate(selected, color);

	//Backpropaging
	BackPropage(selected);
}

Intersection Go::GetBestMove(long seconds, int color) {
	//Run the algorithm
	clock_t start_time=clock();
    while((long) ((clock() - start_time) / CLOCKS_PER_SEC) < seconds) {
		MontecarloAlgorithm (color);
	}

	//Retrieve the answer
	if(root_->kodomo_.size() > 0) {
		float score = (static_cast<float>(root_->kodomo_[0].winCounter_));
		int best=0;
		for(int i=0, lg=root_->kodomo_.size(); i<lg; ++i) {
			if(score < static_cast<float>(root_->kodomo_[i].winCounter_)) {
				best = i;
				score = static_cast<float>(root_->kodomo_[i].winCounter_);
			}
		}
		return root_->moves_[best];
	}
	else {
		cerr << "The root has no child, cannot find the best move amongst them!" << endl;
		cout << "An error has occured" << endl;
		return NULL;
	}
}
		
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
		root_ = &(root_->kodomo_[i]);
		root_->becomeRoot();
	}
	else {
		cerr << "Could not find the proper child, try running montecarlo algorithm before using the method again" << endl;
	}


}

Node& Go::Select(Node& explored) {
	while(!explored.isLeaf()) {
		Node& best = explored;
		float max = numeric_limits<float>::min();
		for(vector<Node>::iterator it = explored.kodomo_.begin(); it != explored.kodomo_.end(); ++it) {
			float score = (static_cast<float>(it->winCounter_) / it->playCounter_) 
						+ C * sqrt(log((float)explored.playCounter_) / it->playCounter_);
			if(score > max) {
				max = score;
				best = explored;
			}
		}
		explored = best;
	}

	return explored;
}

Node& Go::Expand(Node& node, int color) {
	// Create the list of the children
	for (int i = 0; i < Taille; ++i) {
		for (int j = 0; j < Taille; ++j) {
			Intersection i(i,j);
			if(coupLegal(i, color)) {
				node.moves_.push_back(i);
				node.kodomo_.push_back(Node(&node));
			}
		}
	}

	if(node.kodomo_	.size() > 0)
		return node.kodomo_.front();
	else
		return node;
}

void Go::Simulate(Node& node, int color) {
	//We remember the old goban so that we can restore it afterwards
	char *old = CopyGoban(goban);
	//We play the random playout
	playout(color);
	//The node has so been played once more...
	++node.playCounter_;
	//.. and its winCounter has possibly evolved
	node.winCounter_ += score[color];
	//We delete the resulting goban...
	delete[] goban;
	//...and eventually restore the previous goban
	goban = old;
}

void Go::BackPropage(Node& node) {
	int play = node.playCounter_;
	int win = node.winCounter_;
	while(!node.isRoot()) {
		node = node.parent_;
		node.winCounter_ += win;
		node.playCounter_ += play;
	}
}