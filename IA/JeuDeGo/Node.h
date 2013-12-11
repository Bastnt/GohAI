#ifndef NODE_H
#define NODE_H

#include "Intersection.h"
#include "Marquage.h"
#include <list>

using namespace std;

class Node {
	public :
		int winCounter_, playCounter_;
		Node* parent_;
		list<Intersection> moves_;
		list<Node> kodomo_;
		unsigned long long hash_;

		Node(Node* parent = NULL);
		Node(const Node & n);
		void becomeRoot();
		bool isRoot();
		bool isLeaf();
		int getScore();
};

#endif