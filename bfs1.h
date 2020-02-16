#ifndef BFS1_H
#define BFS1_H

#include <queue>
#include <stdint.h>


enum class Color{
    white = 0,
    grey,
    black
};

//forward declare to have the typedef
class Vertex;
typedef std::vector<Vertex*> Graph;


class Vertex{
public:
    Graph childs;
    Graph parents;
    uint64_t dbId;
    Color color;

};


/**
 * @brief The BFS1Observer class
 */
class BFS1Observer {
public:
    void examineVertex(Vertex* vertex);
    void examineEdge(Vertex* parent, Vertex* child);;
    void treeEdge(Vertex* parent, Vertex* child);;
};

struct ChainResult{

};

class BFS1 {
      public:
    BFS1();
    /**
     * @brief getChain
     * @param IdStart
     * @param IdEnd
     * @param maxDepth we do not care of things that are too far
     * @param forward from parent to child
     * @return the list of Vertex in order
     */
    Graph getChain(uint64_t IdStart, uint64_t IdEnd, uint64_t maxDepth, bool forward = true);
    //TODO get all possible chain ???
private:
    Graph graph;
    BFS1Observer* observer;
    //64bit and you avoid problem
    std::queue<uint64_t> q;
};

#endif // BFS1_H
