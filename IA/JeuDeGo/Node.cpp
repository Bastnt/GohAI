#include "Node.h"

Node::Node(Node* parent) : parent_(parent) {
	winCounter_ = 0;
	playCounter_ = 0;
}

Node::Node(const Node & n) {
	winCounter_ = n.winCounter_;
	playCounter_ = n.playCounter_;
	parent_ = n.parent_;
	for(int i=0, lg=n.kodomo_.size(); i<lg; ++i) {
		kodomo_.push_back(n.kodomo_[i]);
	}
	for(int i=0, lg=n.moves_.size(); i<lg; ++i) {
		moves_.push_back(n.moves_[i]);
	}
	hash_ = n.hash_;
}

void Node::becomeRoot() {
	parent_=NULL;
}

bool Node::isRoot() {
	return parent_==NULL;
}

bool Node::isLeaf() {
	return kodomo_.size()==0;
}

int Node::getScore() {
	//TODO
	return 0;
}