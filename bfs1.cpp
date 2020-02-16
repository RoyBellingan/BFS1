#include "bfs1.h"
#include <stack>
using namespace std;
BFS1::BFS1()
{
    Vertex v1;
    v1.color = Color::grey;
}

void BFS1Observer::examineVertex(Vertex *vertex) {
    (void)vertex;
}

void BFS1Observer::examineEdge(Vertex *parent, Vertex *child){
    (void)parent;
    (void)child;
}

void BFS1Observer::treeEdge(Vertex *parent, Vertex *child) {
    (void)parent;
    (void)child;
}
