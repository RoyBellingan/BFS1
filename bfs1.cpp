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
}

void BFS1::resolve(uint64_t IdStart, uint64_t maxIteration, bool forward) {
    start(IdStart);

    //taken from boost::graph::breadth_first_search:75
    //iteracion limiter is practically the only addition
    uint64_t iteration = 0;
    while (!q.empty()) {
        if(maxIteration && iteration > maxIteration){
            break;
        }
        iteration++;

        Vertex* parent = q.front();
        q.pop();

        if (visitor) {
            bool ok = visitor->examineVertex(parent);
            if(!ok){
                continue;
            }
        }

        for (Vertex* child : parent->childs) {

            call(examineEdge(parent, child));

            if (child->color == Color::white) {
                call(treeEdge(parent, child));
                child->color = Color::grey;
                call(discoverVertex(child));
                q.push(child);
            } else {
                call(nonTreeEdge(parent, child));
                if (child->color == Color::grey) {
                    call(grayTarget(parent, child));
                } else {
                    call(blackTarget(parent, child));
                }
            }
        } // end for
        parent->color = Color::black;
        call(finishVertex(parent));
    }// end while
}

void BFS1::whiteWash() {
    for (auto&& line : graph) {
        line.second->color = Color::white;
    }
}

void BFS1::start(uint64_t IdStart) {
    whiteWash();
    auto v   = graph.at(IdStart);
    v->color = Color::grey;
    call(discoverVertex(v));
    q.push(v);
}

bool Visitor::examineVertex(Vertex* vertex) {
    (void)vertex;
    return true;
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

void Visitor::finishVertex(Vertex *parent)
{
    (void)parent;
}
