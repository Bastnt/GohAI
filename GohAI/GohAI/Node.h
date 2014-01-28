#ifndef NODE_H
#define NODE_H

#include "Move.h"
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
		vector<Move> moves_;
		//The list of children Nodes
		vector<Node*> children_;

		Node(Node* parent = NULL);
		Node(const Node & n);

		bool isRoot();
		bool isLeaf();
};

#endif