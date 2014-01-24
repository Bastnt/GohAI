#include "Go.h"

using namespace std;

const float Go::C = 0.3f;

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
	/*goban[c(1,1)] = Noir;
	goban[c(2,3)] = Blanc;
	goban[c(4,2)] = Noir;
	goban[c(1,2)] = Blanc;*/
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
	//(0;0) means skip a turn, always a legal move
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

	
	if (score [Blanc] > score [Noir]) {
		score [Blanc] = 1.0;
		score [Noir] = 0.0;
	}
	else {
		score [Blanc] = 0.0;
		score [Noir] = 1.0;
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

		joue(inter, couleur);
		if (couleur == Noir)
			couleur = Blanc;
		else
			couleur = Noir;
	}
	calculeScores ();
}

void Go::play (Intersection& move, int couleur) {
	while(true) {
		if ((nbCoupsJoues >= MaxCoups) || gameOver ())
			break;

		joue(move, couleur);
		if (couleur == Noir)
			couleur = Blanc;
		else
			couleur = Noir;
	}
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
	char* dest = new char[WIDTH*WIDTH];
	strncpy(dest, src, WIDTH*WIDTH);
	return dest;
}

Intersection Go::GetBestMove(long seconds, int color) {
	//Run the algorithm
	clock_t start_time=clock();
    while((long) ((clock() - start_time) / CLOCKS_PER_SEC) < seconds) {
		MontecarloAlgorithm(color);
	}

	//Retrieve the answer
	if(root_->kodomo_.size() > 0) {
		float score = (static_cast<float>(root_->kodomo_[0].winCounter_));
		int best=0;
		for(int i=1, lg=root_->kodomo_.size(); i<lg; ++i) {
			if(score < static_cast<float>(root_->kodomo_[i].winCounter_)) {
				best = i;
				score = static_cast<float>(root_->kodomo_[i].winCounter_);
			}
		}
		cout << "The next move to perform is: (" << root_->moves_[best].x_ << ";" << root_->moves_[best].y_ << ")" << endl;
		return root_->moves_[best];
	}
	else {
		cerr << "The root has no child, cannot find the best move amongst them!" << endl;
		cout << "An error has occured, please consult your log" << endl;
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

Node& Go::Select(Node& root, int& color) {
	Node * best = &root;
	//Remember the position within the kodomo to update the goban with the good move 
	while(!best->isLeaf()) {
		int best_index=-1;
		float max = -50.0f, score;
		for(int i=0, lg=best->kodomo_.size(); i<lg; ++i) {
			if(best->kodomo_[i].playCounter_==0) {
				return best->kodomo_[i];
			}
			else
			score = (static_cast<float>(best->kodomo_[i].winCounter_) / best->kodomo_[i].playCounter_)
					+ C * sqrt(log((float)root.playCounter_) / best->kodomo_[i].playCounter_);
			if(score > max) {
				max = score;
				best_index=i;
			}
		}
		//update the gohan according to the color
		best = &(best->kodomo_[best_index]);
		play(best->moves_[best_index],color);
	}
	return *best;
}

Node& Go::Expand(Node& node, int& color) {
	color = color==Case::Blanc ? Case::Noir : Case::Blanc;
	// Create the list of the children
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			Intersection inter(i,j);
			if(coupLegal(inter, color)) {
				node.moves_.push_back(inter);
				Node n(&node);
				node.kodomo_.push_back(n);
			}
		}
	}

	if(node.kodomo_.size() > 0)
		return node.kodomo_.front();
	cout << "WARNING: Could not find any child, returned the node itself" << endl;
	cerr << "WARNING: Could not find any child, returned the node itself" << endl;
	return node;
}

void Go::Simulate(Node& node, int color) {
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
}

void Go::BackPropage(Node& node) {
	int play = node.playCounter_;
	int win = node.winCounter_;
	Node* current = &node;
	while(!current->isRoot()) {
		current = current->parent_;
		current->winCounter_ += win;
		current->playCounter_ += play;
	}
}

void Go::MontecarloAlgorithm(int root_color) {
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
	for (int i = 1; i <= Taille; i++) {
		for (int j = 1; j <= Taille; j++) {
			switch(goban [c(i,j)]) {
			case Vide: 
				cout << "V";
				break;
			case Blanc: 
				cout << "B";
				break;
			case Noir: 
				cout << "N";
				break;
			case Exterieur:
				cout << "E";
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
	save_goban = CopyGoban(goban);
	save_nbCoupsJoues = nbCoupsJoues;
	save_hash = hash;
	//moves and HashHistory are saved due to nbCoupsJoues
	save_dejavu = dejavu;
	save_dejavu2 =dejavu2;
}

void Go::RestoreState(){
	goban = CopyGoban(save_goban);
	nbCoupsJoues = save_nbCoupsJoues;
	hash = save_hash;
	dejavu =save_dejavu;
	dejavu2 = save_dejavu2;
}