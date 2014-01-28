#include "Node.h"

Node::Node(Node* parent, Move move) : parent_(parent), move_(move), winCounter_(0), playCounter_(0) {}

bool Node::isRoot() {
	return parent_==NULL;
}

bool Node::isLeaf() {
	return children_.size()==0;
}