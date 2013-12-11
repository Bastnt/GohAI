#include "Node.h"

Node::Node(int won, int parcoured, Node* parent) : winCounter_(won), visitCounter_(parcoured), parent_(parent) {
}

Node::Node(const Node & n) {
	winCounter_ = n.winCounter_;
	parent_ = n.parent_;
	//moves_(n.moves_);
	//kodomo_(n.kodomo_);
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