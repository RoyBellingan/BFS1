#include "bfs1.h"

using namespace std;
using namespace BFS;

/*
 * Used to avoid check if visitor ptr is valid, and than call the related function
 * clang format place the \ in wild position
*/
#define call(val)             \
	if ((visitor)) {      \
		visitor->val; \
	}

BFS1::BFS1() {
	Vertex v1;
	v1.color = Color::grey;
	//a mere 800mb for now
	//graph.resize(120000000);
	graph.resize(1000);
}

void BFS1::resolve(uint64_t IdStart, bool forward) {
	this->forward = forward;
	start(IdStart);

	//taken from boost::graph::breadth_first_search:75
	//iteracion limiter is practically the only addition
	while (!q.empty()) {
		Vertex* node = q.front();
		q.pop();
		dirtyVertex.push_back(node);

		call(examineVertex(node));

		auto& iterateMe = node->childs;
		if (!forward) {
			iterateMe = node->parents;
		}

		for (Vertex* leaf : iterateMe) {
			if (terminate) {
				break;
			}
			call(examineEdge(node, leaf));

			if (leaf->color == Color::white) {
				call(treeEdge(node, leaf));
				leaf->color = Color::grey;
				call(discoverVertex(leaf));
				q.push(leaf);
			} else {
				call(nonTreeEdge(node, leaf));
				if (leaf->color == Color::grey) {
					call(grayTarget(node, leaf));
				} else {
					call(blackTarget(node, leaf));
				}
			}
		} // end for
		node->color = Color::black;
		call(finishVertex(node));
	} // end while
}

Visitor* BFS1::getVisitor() const {
	return visitor;
}

void BFS1::setVisitor(Visitor* value) {
	visitor       = value;
	visitor->bfs1 = this;
}

void BFS1::whiteWash() {
	for (auto&& line : dirtyVertex) {
		line->color = Color::white;
	}
}

void BFS1::start(uint64_t IdStart) {
	whiteWash();
	auto v   = graph.at(IdStart);
	v->color = Color::grey;
	call(discoverVertex(v));
	q.push(v);
}

void Visitor::examineVertex(Vertex* vertex) {
	(void)vertex;
}

void Visitor::examineEdge(Vertex* parent, Vertex* child) {
	(void)parent;
	(void)child;
}

void Visitor::treeEdge(Vertex* parent, Vertex* child) {
	(void)parent;
	(void)child;
}

void Visitor::discoverVertex(Vertex* vertex) {
	(void)vertex;
}

void Visitor::nonTreeEdge(Vertex* parent, Vertex* child) {
	(void)parent;
	(void)child;
}

void Visitor::grayTarget(Vertex* parent, Vertex* child) {
	(void)parent;
	(void)child;
}

void Visitor::blackTarget(Vertex* parent, Vertex* child) {
	(void)parent;
	(void)child;
}

void Visitor::finishVertex(Vertex* parent) {
	(void)parent;
}
