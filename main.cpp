
#include "bfs1.h"
#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string.h>
#include <valgrind/callgrind.h>

double nanotime() {
    return (double(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000000));
}
using namespace boost;

//OOP win over template madness IMHO
class VertexDist : public BFS::Vertex {
      public:
    uint16_t dist = 0;
    //std::string name;
};

class VisitorT1 : public BFS::Visitor {
      public:
    BFS::Graph* graph     = nullptr;
    bool        terminate = false;

    VisitorT1() {
    }

    bool examineVertex(BFS::Vertex* vertex) override {
        (void)vertex;
        return !terminate;
    }
    /**
     * This static_cast thing is not very elegant to write...
     * here template could have helped to have a nicer sintax, or maybe have moved ugliness elsewhere ?
     * @brief treeEdge
     * @param parent
     * @param child
     */
    void treeEdge(BFS::Vertex* parent, BFS::Vertex* child) override {
        auto c  = static_cast<VertexDist*>(child);
        auto p  = static_cast<VertexDist*>(parent);
        c->dist = p->dist + 1;
        if(c->dist > 10){
            terminate = true;
        }
    }
};

uint64_t nonInsanelySlowStringToUint(const char* p) {
    uint64_t x   = 0;
    bool     neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        x = (x * 10) + (*p - '0');
        ++p;
    }
    if (neg) {
        x = -x;
    }
    return x;
}

std::pair<uint64_t, uint64_t> split(std::string_view& line) {
    std::string_view view = line;
    auto             pos  = line.find(',');
    auto             a    = view.substr(0, pos);
    auto             b    = view.substr(pos + 1);
    auto             ad   = nonInsanelySlowStringToUint(a.data());
    auto             bd   = nonInsanelySlowStringToUint(b.data());
    return std::pair{ad, bd};
}

int main() {
    BFS::BFS1 bfs1;
    auto      visT1 = new VisitorT1();
    bfs1.visitor    = visT1;
    visT1->graph    = &bfs1.graph;

    FILE* datafile;
    datafile = fopen("kw1.csv","r");
    if (datafile == NULL){
        std::cerr << "No /tmp/kw1.csv file" << std::endl;
        return EXIT_FAILURE;
    }

    uint64_t maxId = 5000;
    int      run   = 0;
    char*    luv   = (char*)malloc(65000);
    while (true) {
        size_t len = 0;

        if (fgets(luv, 65000, datafile) == nullptr) {
            break;
        }
        run++;
        std::string_view line(luv, strlen(luv));
        auto             v = split(line);

        uint64_t parentId = v.first;
        uint64_t childId  = v.second;

        if (maxId && parentId > maxId) {
            break;
        }

        VertexDist *v1 = nullptr, *v2 = nullptr;
        {
            v1 = static_cast<VertexDist*>(bfs1.graph[parentId]);
            if (v1 == nullptr) {
                v1                   = new VertexDist();
                v1->dbId             = parentId;
                bfs1.graph[parentId] = v1;
            }
            //            auto iter = bfs1.graph.find(parentId);
            //            if (iter == bfs1.graph.end()) {
            //                v1       = new VertexName();
            //                v1->dbId = parentId;
            //                bfs1.graph.insert({parentId, v1});
            //            } else {
            //                v1 = static_cast<VertexName*>(iter->second);
            //            }
        }

        {
            v2 = static_cast<VertexDist*>(bfs1.graph[childId]);
            if (v2 == nullptr) {
                v2                  = new VertexDist();
                v2->dbId            = childId;
                bfs1.graph[childId] = v2;
            }

            //            auto iter = bfs1.graph.find(childId);
            //            if (iter == bfs1.graph.end()) {
            //                v2       = new VertexName();
            //                v2->dbId = childId;
            //                bfs1.graph.insert({childId, v2});
            //            } else {
            //                v2 = static_cast<VertexName*>(iter->second);
            //            }
        }

        //dual indexing power supreme
        v1->childs.push_back(v2);
        v2->parents.push_back(v1);
    }

    std::cout << "run " << run << "\n";
    CALLGRIND_START_INSTRUMENTATION;
    //        {
    //            auto t1 = nanotime();
    //            bfs1.resolve(4);
    //            auto t2 = nanotime();
    //            std::cout << "elapsed " << t2 - t1 << "\n";
    //        }
    {
        auto t1 = nanotime();
        bfs1.resolve(95);
        auto t2 = nanotime();
        std::cout << "elapsed " << t2 - t1 << "\n";
    }
    //    {
    //        auto t1 = nanotime();
    //        bfs1.resolve(95);
    //        auto t2 = nanotime();
    //        std::cout << "elapsed " << t2 - t1 << "\n";
    //    }
    //    {
    //        auto t1 = nanotime();
    //        bfs1.resolve(5);
    //        auto t2 = nanotime();
    //        std::cout << "elapsed " << t2 - t1 << "\n";
    //    }
    std::ios::sync_with_stdio(true);
    for (auto line : bfs1.graph) {
        auto v = static_cast<VertexDist*>(line);
        if (!v || v->dist == 0) {
            continue;
        }

        std::cout << v->dbId << " has a Bacon number of "
                  << v->dist << "\n";
    }

    //            for (auto line : visT1->distanceMap) {
    //                auto v = static_cast<VertexName*>(bfs1.graph.at(line.first));
    //                std::cout << v->dbId << " has a Bacon number of "
    //                          << line.second << "\n";
    //            }

    return 0;
}
