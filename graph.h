#ifndef __GRAPH__
#define __GRAPH__

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <fstream>
#include <set>
#include <random>
#include <ppl.h>
#include <concurrent_unordered_set.h>
using namespace std;
using namespace concurrency;

class vertex{
public:
	vertex(int label){
		this->label = label;
	}
	void addAdjacent(int label){
		adj.push_back(label);
	}

	int label;
	vector<int> adj;
};

typedef pair<int, int> edge;


#define READ_ADJ 0
#define READ_MAT 1
class graph{
public:
	//Default constructor
	graph();
	//Construct graph with initial vertices set
	graph(set<int> v);
	graph(concurrent_unordered_set<int> v);
	//Generate graph from file
	//file format
	//  int vertex number
	//  int edge number
	//  int u int v(edge from u to v)
	//  .
	//  .
	//  .
	///////////////////////
	graph(string filename, int type=READ_ADJ);
	void addEdge(int u, int v);
	//Random file generator
	void rndGraph(int vn, int en);
	//Return true if graph has no vertices
	bool isEmpty();
	//Return vertices number
	int vertices_number();
	//Return edges number
	int edge_number();
	//Return index of label
	int getidx(int label);
	//Dump graph data
	void dump();
	//Return subgraph induced by vertices set v
	graph subgraph(set<int> v);
	graph subgraph(concurrent_unordered_set<int> v);
	//Verify mis s if it is valid
	bool verifyMis(set<int> s);

	vector<vertex> vertices;
	vector<edge> edges;
	map<int, int> idxMap;
};

#endif