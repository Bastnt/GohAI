#include "Go.h"

using namespace std;

Go::Go () {
	hash = 0;
	nbCoupsJoues = 0;
	for (int i = 1; i <= Taille; i++)
		for (int j = 1; j <= Taille; j++)
			goban [i] [j] = Vide;
	for (int i = 0; i < Taille + 2; i++) {
		goban [0] [i] = Exterieur;
		goban [i] [0] = Exterieur;
		goban [Taille + 1] [i] = Exterieur;
		goban [i] [Taille + 1] = Exterieur;
	}
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
	if ((inter._x == 0) && (inter._y == 0))
		return true;

	if (goban [inter._x] [inter._y] != Vide)
		return false;

	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if (goban [voisine._x] [voisine._y] == Vide)
			return true;
	}

	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if (goban [voisine._x] [voisine._y] == couleur)
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
	int adversaire = Noir;
	if (couleur == Noir)
		adversaire = Blanc;

	dejavu2.init ();
	dejavu2.marque (inter);
	h ^= HashArray [couleur] [inter._x] [inter._y];
	h ^= HashTurn;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if (!dejavu2.marquee (voisine)) {
			if (goban [voisine._x] [voisine._y] == adversaire)
				if (minLib (voisine, 2) == 1) {
					stack<Intersection> st;
					dejavu2.marque (voisine);
					st.push (voisine);
					while (!st.empty ()) {
						Intersection courante = st.top ();
						st.pop ();
						h ^= HashArray [goban [voisine._x] [voisine._y]] [courante._x] [courante._y];
						for (int j = 0; j < 4; j++) {
							Intersection pierre = courante.voisine (j);
							if (goban [pierre._x] [pierre._y] == adversaire)
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
	int compteur = 0, couleur = goban [inter._x] [inter._y];

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
				if (goban [voisine._x] [voisine._y] == Vide) {
					compteur++;
					if (compteur >= min)
						return compteur;
				}
				else if (goban [voisine._x] [voisine._y] == couleur)
					st.push (voisine);
			}
		}
	}
	return compteur;
}

int Go::minLibIfPlay (Intersection intersection, int couleur, int min) {
	stack<Intersection> st;
	int compteur = 0;

	if (goban [intersection._x] [intersection._y] == Vide) {
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
					if (goban [voisine._x] [voisine._y] == Vide) {
						compteur++;
						if (compteur >= min)
							return compteur;
					}
					if (goban [voisine._x] [voisine._y] == couleur)
						st.push (voisine);
				}
			}
		}
		int adversaire = Noir;
		if (couleur == Noir)
			adversaire = Blanc;
		for (int i = 0; i < 4; i++) {
			Intersection voisine = intersection.voisine (i);
			if (goban [voisine._x] [voisine._y] == adversaire)
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

	if (inter._x != 0) {
		posePierre (inter, couleur);
		enlevePrisonniers (inter, couleur);
	}

	nbCoupsJoues++;
}

void Go::posePierre (Intersection inter, int couleur) {
	goban [inter._x] [inter._y] = couleur;
	hash ^= HashArray [couleur] [inter._x] [inter._y];
}

void Go::enlevePrisonniers (Intersection inter, int couleur) {
	stack<Intersection> st;
	int adversaire = Noir;
	if (couleur == Noir)
		adversaire = Blanc;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = inter.voisine (i);
		if ((goban [voisine._x] [voisine._y] == adversaire))
			if (minLib (voisine, 1) == 0)
				st.push (voisine);
	}
	while (!st.empty ()) {
		Intersection voisine = st.top ();
		st.pop ();
		if ((goban [voisine._x] [voisine._y] == adversaire))
			enleveChaine (voisine);
	}
}

void Go::enleveChaine (Intersection intersection) {
	stack<Intersection> st;
	int couleur = goban [intersection._x] [intersection._y];

	st.push (intersection);
	while (!st.empty ()) {
		Intersection inter = st.top ();
		st.pop ();
		hash ^= HashArray [couleur] [inter._x] [inter._y];
		goban [inter._x] [inter._y] = Vide;
		for (int i = 0; i < 4; i++) {
			Intersection voisine = inter.voisine (i);
			if ((goban [voisine._x] [voisine._y] == couleur))
				st.push (voisine);
		}
	}
}

bool Go::entouree (Intersection intersection, int couleur) {
	if (goban [intersection._x] [intersection._y] != Vide)
		return false;
	for (int i = 0; i < 4; i++) {
		Intersection voisine = intersection.voisine (i);
		if ((goban [voisine._x] [voisine._y] != couleur) &&
			(goban [voisine._x] [voisine._y] != Exterieur))
			return false;
	}
	for (int i = 0; i < 4; i++) {
		Intersection voisine = intersection.voisine (i);
		if ((goban [voisine._x] [voisine._y] == couleur))
			if (minLib (voisine, 2) == 1)
				return false;
	}
	return true;
}

bool Go::protegee (Intersection inter, int couleur, bool & bord) {
	if (goban [inter._x] [inter._y] == Exterieur) {
		bord = true;
		return true;
	}
	if (goban [inter._x] [inter._y] == Vide)
		if (entouree (inter, couleur))
			return true;
	if (goban [inter._x] [inter._y] == couleur)
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
	score [Blanc] = komi;

	for (int i = 1; i <= Taille; i++)
		for (int j = 1; j <= Taille; j++)
			if (goban [i] [j] == Vide) {
				Intersection inter (i, j);
				if (entouree (inter, Noir))
					score [Noir] += 1.0;
				else if (entouree (inter, Blanc))
					score [Blanc] += 1.0;
			}
			else  
				score [goban [i] [j]] += 1.0;

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
	while ((last > 0) && (moves [last]._x == 0)) {
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
			if (goban [i] [j] == Vide) {
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

				nbUrgences -= urgence [inter._x] [inter._y];
				urgence [inter._x] [inter._y] = 0;
	}
	while (entouree (inter, couleur) || 
		!coupLegal (inter, couleur));

	return inter;
}

void Go::playout (int couleur) {
	for (;;) {
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
