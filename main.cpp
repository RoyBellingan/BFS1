
#include "bfs1.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <string>
#include <valgrind/callgrind.h>

double nanotime() {
	return (double(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000000));
}

std::vector<std::string_view> csvSplit(const std::string_view str, const char delim = ',') {
	std::vector<std::string_view> result;

	int indexCommaToLeftOfColumn  = 0;
	int indexCommaToRightOfColumn = -1;

	for (int i = 0; i < static_cast<int>(str.size()); i++) {
		if (str[i] == delim) {
			indexCommaToLeftOfColumn  = indexCommaToRightOfColumn;
			indexCommaToRightOfColumn = i;
			int index                 = indexCommaToLeftOfColumn + 1;
			int length                = indexCommaToRightOfColumn - index;

			std::string_view column(str.data() + index, length);
			result.push_back(column);
		}
	}
	const std::string_view finalColumn(str.data() + indexCommaToRightOfColumn + 1, str.size() - indexCommaToRightOfColumn - 1);
	result.push_back(finalColumn);
	return result;
}

//OOP win over template madness IMHO
class VertexDist : public BFS::Vertex {
      public:
	uint16_t     dist      = 0;
	bool         isRelated = true;
	BFS::Related chain;

	std::string unchain() const {
		std::string buf;
		for (auto v : chain) {
			buf.append(std::to_string(v->dbId)).append(" -> ");
		}
		buf.append(std::to_string(dbId));
		return buf;
	}
	void enchain(VertexDist* parent) {
		//copy prev chain (chain can be empty, so begin do not exists -.-)
		std::copy(parent->chain.begin(), parent->chain.end(), std::back_inserter(chain));
		//and append cur
		chain.push_back(parent);
	}
};

class VisitorT1 : public BFS::Visitor {
      public:
	BFS::Graph* graph = nullptr;
	VisitorT1() {
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

		c->enchain(p);

		//in case we have no "close" related search, we just ignore and use the old method with the CTE
		//if fact we simply do not update as is irrelevant at this point the relationship...
		//we just place in a 10 depth bucket all of them to signal they have very low priority
		if (c->dist > 10) {
			bfs1->terminate = true;
		}
		//This will stop searching for other chain ! for now is ok
		if (!c->isRelated) {
			bfs1->terminate = true;
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

struct Line {
	uint64_t parentId;
	uint64_t childId;
	bool     isRelated;
};

Line split(std::string_view& line) {
	auto csv = csvSplit(line);
	if (csv.size() != 3) {
		throw std::string("invalid line size for").append(line);
	}
	Line resLine;
	resLine.parentId  = nonInsanelySlowStringToUint(csv[0].data());
	resLine.childId   = nonInsanelySlowStringToUint(csv[1].data());
	resLine.isRelated = !csv[2].compare("3");
	return resLine;
}

int main() {
	BFS::BFS1 bfs1;
	auto      visT1 = new VisitorT1();
	bfs1.setVisitor(visT1);
	visT1->graph = &bfs1.graph;

	FILE* datafile;
	/**
	  Generated from something like
	  the user need to have the file privileges to be able to execute that
	  and than copied where this program is running, this part is normally handled by an external script

	  The CSV creation takes around 2Min for 70M row, leading to 1.4Gb file
SELECT
    *
FROM
    scraper.relatedKeywordView1
INTO OUTFILE '/home/roy/kw1.csv'
	FIELDS TERMINATED BY ','
	LINES TERMINATED BY '\n';

	This file will be placed on MYSQL MACHINE (not locally)

	We should also have in some way the MAX value to expect, so we can resize just once the vector, for now we do that dynamically
	  */
	auto fileName = "kw2.csv";
	datafile      = fopen(fileName, "r");
	if (datafile == NULL) {
		std::cerr << fileName << std::endl;
		return EXIT_FAILURE;
	}

	uint64_t maxId = 1000000;
	int      run   = 0;
	char*    luv   = (char*)malloc(65000);
	while (true) {
		if (fgets(luv, 65000, datafile) == nullptr) {
			break;
		}
		run++;
		if (strlen(luv) < 3) { //protect against empty lines
			continue;
		}
		std::string_view line(luv, strlen(luv) - 1);
		auto             v = split(line);

		if (maxId && v.parentId > maxId) {
			break;
		}

		VertexDist *v1 = nullptr, *v2 = nullptr;
		{
			v1 = static_cast<VertexDist*>(bfs1.graph[v.parentId]);
			if (v1 == nullptr) {
				v1                     = new VertexDist();
				v1->dbId               = v.parentId;
				v1->isRelated          = v.isRelated;
				bfs1.graph[v.parentId] = v1;
			} else { //to avoid swapping two colum to save immediately, whe always refresh, much cheaper and quicker than write double stuff
				v1->isRelated = v.isRelated;
			}
		}

		{
			v2 = static_cast<VertexDist*>(bfs1.graph[v.childId]);
			if (v2 == nullptr) {
				v2                    = new VertexDist();
				v2->dbId              = v.childId;
				bfs1.graph[v.childId] = v2;
			}
		}

		//dual indexing power supreme
		v1->childs.push_back(v2);
		v2->parents.push_back(v1);
	}

	std::cout << "run " << run << "\n";
	CALLGRIND_START_INSTRUMENTATION;
	{
		auto t1 = nanotime();
		bfs1.resolve(5, false);
		auto t2 = nanotime();
		std::cout << "elapsed " << t2 - t1 << "\n";
	}

	std::ios::sync_with_stdio(true);
	for (auto line : bfs1.graph) {
		auto v = static_cast<VertexDist*>(line);
		if (!v || v->dist == 0) {
			continue;
		}
		//take only original KW, this is the shortest path, 90% sure
		if(!v->isRelated){
			std::cout << v->dist << " step in chain " << v->unchain() << "\n";
		}
	}
	return 0;
}
