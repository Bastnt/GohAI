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
		//List of moves that lead to the children Nodes
		vector<Move> moves_;
		//List of children Nodes
		vector<Node*> children_;
		//Default constructor
		Node(Node* parent = NULL);
		//Copycat constructor
		Node(const Node & n);
		//Checks if the Node is the root of the tree (has no parent)
		bool isRoot();
		//Checks if the Node is a leaf of the tree (has no child)
		bool isLeaf();
};

#endif