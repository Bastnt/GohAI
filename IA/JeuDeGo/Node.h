#ifndef NODE_H
#define NODE_H

#include "Intersection.h"
#include "Marquage.h"
#include <vector>

class Node {
	public :
		int winCounter_, visitCounter_;
		Node* parent_;
		std::vector<Intersection> moves_;
		std::vector<Node*> kodomo_;
		unsigned long long hash_;

		Node(int win = 0, int visit = 0, Node* parent = NULL);
		Node(const Node & n);
		void becomeRoot();
		bool isRoot();
		bool isLeaf();
		int getScore();
};

#endif