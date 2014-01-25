#ifndef NODE_H
#define NODE_H

#include "Intersection.h"
#include "Marquage.h"
#include <vector>

using namespace std;

class Node {
	public :
		//The number of times this Node has lead to a victorious melee
		int winCounter_;
		//The number of times this Node has been passed by
		int playCounter_;
		//The parent of the current node, NULL if the root
		Node* parent_;
		//The list of moves that lead to the children Nodes
		vector<Intersection> moves_;
		//The list of children Nodes
		vector<Node*> kodomo_;
		unsigned long long hash_;

		Node(Node* parent = NULL);
		Node(const Node & n);

		void becomeRoot();
		bool isRoot();
		bool isLeaf();
};

#endif