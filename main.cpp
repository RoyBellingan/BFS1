
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "bfs1.h"

using namespace boost;

//struct my_visitor : boost::default_bfs_visitor{

//    void initialize_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
//        std::cout << "Initialize: " << g[s] << std::endl;
//    }
//    void discover_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
//        std::cout << "Discover: " << g[s] << std::endl;
//    }
//    void examine_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
//        std::cout << "Examine vertex: " << g[s] << std::endl;
//    }
//    void examine_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
//        std::cout << "Examine edge: " << g[e] << std::endl;
//    }
//    void tree_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
//        std::cout << "Tree edge: " << g[e] << std::endl;
//    }
//    void non_tree_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
//        std::cout << "Non-Tree edge: " << g[e] << std::endl;
//    }
//    void gray_target(const graph_t::edge_descriptor &e, const graph_t &g) const {
//        std::cout << "Gray target: " << g[e] << std::endl;
//    }
//    void black_target(const graph_t::edge_descriptor &e, const graph_t &g) const {
//        std::cout << "Black target: " << g[e] << std::endl;
//    }
//    void finish_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
//        std::cout << "Finish vertex: " << g[s] << std::endl;
//    }
//};

//typedef typename property_traits<ColorMap>::value_type ColorValue;
//typedef color_traits<ColorValue> Color;
template <typename DistanceMap, typename ColorMap>
class bacon_number_recorder : public default_bfs_visitor {
      public:
    bacon_number_recorder(DistanceMap dist, ColorMap map)
        : distanceMap(dist), colorMap(map) {
    }

    template <typename Edge, typename Graph>
    //vis.tree_edge(e, g) is invoked (in addition to examine_edge()) if the edge is a tree edge.
    //The target vertex of edge e is discovered at this time.
    void tree_edge(Edge e, const Graph& g) const {
        typename graph_traits<Graph>::vertex_descriptor
            u          = source(e, g),
            v          = target(e, g);
        distanceMap[v] = distanceMap[u] + 1;
    }

      private:
    DistanceMap      distanceMap;
    mutable ColorMap colorMap;
};

// Convenience function
template <typename DistanceMap, typename ColorMap>
bacon_number_recorder<DistanceMap, ColorMap>
record_bacon_number(DistanceMap d, ColorMap colorMap) {
    return bacon_number_recorder<DistanceMap, ColorMap>(d, colorMap);
}

//OOP win over template madness IMHO
class VertexName : public BFS::Vertex {
      public:
    std::string name;
};

class VisitorT1 : public BFS::Visitor{
public:
    std::unordered_map<uint64_t,uint64_t> distanceMap;
    void treeEdge(BFS::Vertex *parent, BFS::Vertex *child){
        distanceMap[child->dbId] = distanceMap[parent->dbId] + 1;
    }
};

int main() {
    BFS::BFS1 bfs1;
    auto visT1 = new VisitorT1();
    bfs1.visitor = visT1;

    //I do not like this file layout, but is ok for this small test
    std::ifstream datafile("./kevin-bacon.dat");
    if (!datafile) {
        std::cerr << "No ./kevin-bacon.dat file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef adjacency_list<
        vecS,
        vecS,
        undirectedS,
        property<vertex_name_t,
                 std::string>,
        property<edge_name_t,
                 std::string>>
        Graph;

    Graph g;

    typedef property_map<Graph, vertex_name_t>::type actor_name_map_t;
    actor_name_map_t                                 actor_name = get(vertex_name, g);
    typedef property_map<Graph, edge_name_t>::type   movie_name_map_t;
    movie_name_map_t                                 connecting_movie = get(edge_name, g);

    typedef graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::map<std::string, Vertex>          NameVertexMap;
    NameVertexMap                                  actors;

    for (std::string line; std::getline(datafile, line);) {
        char_delimiters_separator<char> sep(false, "", ";");
        tokenizer<>                     line_toks(line, sep);
        tokenizer<>::iterator           i           = line_toks.begin();
        std::string                     actors_name = *i++;
        NameVertexMap::iterator         pos;
        bool                            inserted;
        Vertex                          u, v;
        boost::tie(pos, inserted) = actors.insert(std::make_pair(actors_name, Vertex()));
        VertexName *v1 = nullptr, *v2 = nullptr;
        if (inserted) {
            u             = add_vertex(g);
            actor_name[u] = actors_name;
            pos->second   = u;

            v1       = new VertexName();
            v1->name = actors_name;
            v1->dbId = pos->second;
            bfs1.graph.insert({pos->second, v1});
        } else {
            //No idea why this promotion is required
            v1 = static_cast<VertexName*>(bfs1.graph.at(pos->second));
            u  = pos->second;
        }

        std::string movie_name = *i++;

        boost::tie(pos, inserted) = actors.insert(std::make_pair(*i, Vertex()));
        if (inserted) {
            v             = add_vertex(g);
            actor_name[v] = *i;
            pos->second   = v;

            v2       = new VertexName();
            v2->name = movie_name;
            v2->dbId = pos->second;
            bfs1.graph.insert({pos->second, v2});
        } else {
            v  = pos->second;
            v2 = static_cast<VertexName*>(bfs1.graph.at(pos->second));
        }

        //dual indexing power supreme
        v1->parents.push_back(v2);
        v2->childs.push_back(v1);

        graph_traits<Graph>::edge_descriptor e;
        boost::tie(e, inserted) = add_edge(u, v, g);
        if (inserted)
            connecting_movie[e] = movie_name;
    }
//    for (auto line : bfs1.graph) {
//        auto check = static_cast<VertexName*>(line.second);
//        std::cout << check->name << " @ " << line.first <<"\n";
//    }

    std::vector<int> bacon_number(num_vertices(g));

    Vertex src        = actors["Kevin Bacon"];
    bacon_number[src] = 0;

    //typedef boost::property_map<GraphType, boost::vertex_color_t>::type color_map_t;
    //color_map_t colorMap; //Create a color map

    boost::queue<Vertex> Q;
    auto                 colorMap = boost::make_two_bit_color_map(num_vertices(g), get(vertex_index, g));

    breadth_first_search(g, src,
                         visitor(record_bacon_number(&bacon_number[0], &colorMap)));

    //    breadth_first_search(g,
    //                         src,
    //                         Q,
    //                         record_bacon_number(&bacon_number[0], &colorMap));

    graph_traits<Graph>::vertex_iterator i, end;
    for (boost::tie(i, end) = vertices(g); i != end; ++i) {
        std::cout << actor_name[*i] << " has a Bacon number of "
                  << bacon_number[*i] << std::endl;
    }

    bfs1.resolve(2);
    for(auto line : visT1->distanceMap){
        auto v = static_cast<VertexName*>(bfs1.graph.at(line.first));
        std::cout << v->name << " has a Bacon number of "
                  << line.second << "\n";
    }

    return 0;
}
