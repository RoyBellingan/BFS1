
#include "bfs1.h"
#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <valgrind/callgrind.h>

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
    std::vector<uint64_t> distanceMap;
    //std::unordered_map<uint64_t,uint64_t> distanceMap;
    bool terminate = false;
    VisitorT1() {
        //distanceMap.reserve(50000);
        distanceMap.resize(60042099);
    }

    bool examineVertex(BFS::Vertex* vertex) override {
        (void)vertex;
        return !terminate;
    }

    void treeEdge(BFS::Vertex* parent, BFS::Vertex* child) override {
        auto cur                 = distanceMap[parent->dbId] + 1;
        distanceMap[child->dbId] = cur;
                if (cur > 12) {
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

std::pair<uint64_t, uint64_t> split(std::string& line) {
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

    //no idea but without is much slower
    std::ios::sync_with_stdio(false);
    std::ifstream datafile("/tmp/kw1.csv");
    //std::ifstream datafile("/home/roy/kw2.csv");
    if (!datafile) {
        std::cerr << "No /tmp/kw1.csv file" << std::endl;
        return EXIT_FAILURE;
    }

    uint64_t maxId = 0;
    for (std::string line; std::getline(datafile, line);) {
        auto v = split(line);

        uint64_t parentId = v.first;
        uint64_t childId  = v.second;

        if (maxId && parentId > maxId) {
            break;
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
    uint pos = 0;
        for (auto line : visT1->distanceMap) {
            if(line == 0){
                pos++;
                continue;
            }
            auto v = static_cast<VertexName*>(bfs1.graph.at(pos));
            std::cout << v->dbId << " has a Bacon number of "
                      << line << "\n";
            pos++;
        }

//            for (auto line : visT1->distanceMap) {
//                auto v = static_cast<VertexName*>(bfs1.graph.at(line.first));
//                std::cout << v->dbId << " has a Bacon number of "
//                          << line.second << "\n";
//            }

    return 0;
}
