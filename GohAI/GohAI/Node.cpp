#include "Node.h"

Node::Node(Node* parent) : parent_(parent) {
	winCounter_ = 0;
	playCounter_ = 0;
}

Node::Node(const Node & n) {
	winCounter_ = n.winCounter_;
	playCounter_ = n.playCounter_;
	parent_ = n.parent_;
	for(int i=0, lg=n.children_.size(); i<lg; ++i) {
		children_.push_back(n.children_[i]);
	}
	for(int i=0, lg=n.moves_.size(); i<lg; ++i) {
		moves_.push_back(n.moves_[i]);
	}
}

bool Node::isRoot() {
	return parent_==NULL;
}

bool Node::isLeaf() {
	return children_.size()==0;
}