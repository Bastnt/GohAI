#ifndef NODE_H
#define NODE_H

#include "Move.h"
#include <vector>

using namespace std;

/*
	An information in the Montecarlo Tree
*/
class Node {
	public :
		//Number of times this Node has lead to a victorious melee
		int winCounter_;
		//Number of times this Node has been passed by
		int playCounter_;
		//Parent of the current node, NULL if the root
		Node* parent_;
		//Move that lead to this Node
		Move move_;
		//List of children Nodes
		vector<Node*> children_;

		//Default constructor
		Node(Node* parent, Move move);
		//Checks if the Node is the root of the tree (has no parent)
		bool isRoot();
		//Checks if the Node is a leaf of the tree (has no child)
		bool isLeaf();
};

#endif