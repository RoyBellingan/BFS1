#ifndef BFS1_H
#define BFS1_H

#include <queue>
#include <stdint.h>
#include <unordered_map>

namespace BFS {

enum class Color {
    white = 0,
    grey,
    black
};

//forward declare to have the typedef
class Vertex;

//BFS do not care of name, database Id is all we need here
//typedef std::unordered_map<uint64_t, Vertex*> Graph;

typedef std::vector<Vertex*> Graph;

//child are all the same, right ?
typedef std::vector<Vertex*> Related;

class Vertex {
      public:
    Related  childs;
    Related  parents;
    Color    color = Color::white;
    uint64_t dbId  = 0;
};

/**
 * @brief The BFS1Observer class
 */
class Visitor {
      public:
    virtual bool examineVertex(Vertex* vertex);
    virtual void examineEdge(Vertex* parent, Vertex* child);
    virtual void treeEdge(Vertex* parent, Vertex* child) ;
    virtual void discoverVertex(Vertex* vertex);
    virtual void nonTreeEdge(Vertex* parent, Vertex* child);
    virtual void grayTarget(Vertex* parent, Vertex* child);
    virtual void blackTarget(Vertex* parent, Vertex* child);
    virtual void finishVertex(Vertex* parent);
};

struct ChainResult {
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
    void resolve(uint64_t IdStart,
                     uint64_t maxIteration = 0, bool forward = true);
    //TODO get all possible chain ???
    Graph        graph;
    Visitor* visitor = nullptr;

      private:
    //64bit and you avoid problem
    std::queue<Vertex*> q;
    //Vertex that have been processed, and the color has been changed and need to be reset
    std::vector<Vertex*> dirtyVertex;
    void                 whiteWash();
    void                 start(uint64_t IdStart);
};

} // namespace BFS

#endif // BFS1_H
