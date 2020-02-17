
#include "bfs1.h"
#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
double nanotime() {
    return (double(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000000));
}
using namespace boost;

//OOP win over template madness IMHO
class VertexName : public BFS::Vertex {
      public:
    //std::string name;
};

class VisitorT1 : public BFS::Visitor {
      public:
    std::unordered_map<uint64_t,uint16_t> distanceMap;
    bool terminate = false;
    VisitorT1() {
        distanceMap.reserve(50000);
    }

    bool examineVertex(BFS::Vertex *vertex) override{
        return !terminate;
    }

    void treeEdge(BFS::Vertex* parent, BFS::Vertex* child) {
        auto cur = distanceMap[parent->dbId] + 1;
        distanceMap[child->dbId] = distanceMap[parent->dbId] + 1;
        if(cur > 5){
            terminate = true;
        }
    }
};

int main() {
    BFS::BFS1 bfs1;
    auto      visT1 = new VisitorT1();
    bfs1.visitor    = visT1;

    //std::ifstream datafile("/tmp/kw1.csv");
    std::ifstream datafile("/home/roy/kw2.csv");
    if (!datafile) {
        std::cerr << "No /tmp/kw1.csv file" << std::endl;
        return EXIT_FAILURE;
    }


    uint64_t maxId = 1000000;
    for (std::string line; std::getline(datafile, line);) {
        static char_delimiters_separator<char> sep(false, ",", ",");
        tokenizer<>                            line_toks(line, sep);
        tokenizer<>::iterator                  i        = line_toks.begin();
        uint64_t                               parentId = std::atoll(i->data());
        i++;
        uint64_t childId = std::atoll(i->data());

        if(parentId > maxId){
            continue;
        }

        VertexName *v1 = nullptr, *v2 = nullptr;
        {
            auto iter = bfs1.graph.find(parentId);
            if (iter == bfs1.graph.end()) {
                v1       = new VertexName();
                v1->dbId = parentId;
                bfs1.graph.insert({parentId, v1});
            } else {
                v1 = static_cast<VertexName*>(iter->second);
            }
        }

        {
            auto iter = bfs1.graph.find(childId);
            if (iter == bfs1.graph.end()) {
                v2       = new VertexName();
                v2->dbId = childId;
                bfs1.graph.insert({childId, v2});
            } else {
                v2 = static_cast<VertexName*>(iter->second);
            }
        }

        //dual indexing power supreme
        v1->childs.push_back(v2);
        v2->parents.push_back(v1);
    }

    bfs1.resolve(4);
        for (auto line : visT1->distanceMap) {
            auto v = static_cast<VertexName*>(bfs1.graph.at(line.first));
            std::cout << v->dbId << " has a Bacon number of "
                      << line.second << "\n";
        }

    return 0;
}
