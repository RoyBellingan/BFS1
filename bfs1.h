#ifndef BFS1_H
#define BFS1_H

#include <queue>
#include <stdint.h>
#include <string>

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

class BFS1;
/**
 * @brief The BFS1Observer class
 */
class Visitor {
      public:
	virtual void examineVertex(Vertex* vertex);
	virtual void examineEdge(Vertex* parent, Vertex* child);
	virtual void treeEdge(Vertex* parent, Vertex* child);
	virtual void discoverVertex(Vertex* vertex);
	virtual void nonTreeEdge(Vertex* parent, Vertex* child);
	virtual void grayTarget(Vertex* parent, Vertex* child);
	virtual void blackTarget(Vertex* parent, Vertex* child);
	virtual void finishVertex(Vertex* parent);
	//the Visitor is aware of the caller logic
	BFS1* bfs1 = nullptr;
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
	void resolve(uint64_t IdStart, bool forward = true);
	//TODO get all possible chain ???
	Graph    graph;
	bool     forward   = true;
	bool     terminate = false;

	Visitor *getVisitor() const;
	void setVisitor(Visitor *value);

private:
	Visitor* visitor   = nullptr;
	//64bit and you avoid problem
	std::queue<Vertex*> q;
	//Vertex that have been processed, and the color has been changed and need to be reset
	std::vector<Vertex*> dirtyVertex;
	void                 whiteWash();
	void                 start(uint64_t IdStart);
};

} // namespace BFS

#endif // BFS1_H
