#include "graph.h"

graph::graph(){
}

graph::graph(string filename, int type){
	ifstream gf(filename);
	if(type == READ_ADJ){
		map<edge, bool> etable;
		int vn, en, u, v;
		gf >> vn >> en;
		for(int i=0; i<vn; i++){
			vertices.push_back(vertex(i));
			idxMap[i] = i;
		}
		for(int i=0; i<en; i++){
			gf >> u >> v;
			//edge already exist
			if(etable[edge(u,v)] || etable[edge(v,u)])
				continue;
			etable[edge(u,v)] = true;
			etable[edge(v,u)] = true;
			addEdge(u, v);
		}
	}
	else if(type == READ_MAT){
		int vn;
		gf >> vn;
		for(int i=0; i<vn; i++){
			vertices.push_back(vertex(i));
			idxMap[i] = i;
		}
		for(int i=0; i<vn; i++){
			for(int j=0; j<vn; j++){
				int edge;
				gf >> edge;
				if(j>i && edge==1)
					addEdge(i, j);
			}
		}
	}
	else 
		cout << "Reading type error" << endl;
}

graph::graph(set<int> v){
	int count=0;
	for(set<int>::iterator it=v.begin(); it!=v.end(); it++){
		vertices.push_back(vertex(*it));
		idxMap[*it] = count;
		count++;
	}
}

graph::graph(concurrent_unordered_set<int> v){
	int count=0;
	for(concurrent_unordered_set<int>::iterator it=v.begin(); it!=v.end(); it++){
		vertices.push_back(vertex(*it));
		idxMap[*it] = count;
		count++;
	}
}

void graph::rndGraph(int vn, int en){
	vertices.clear();
	edges.clear();
	for(int i=0; i<vn; i++){
		vertices.push_back(i);
		idxMap[i] = i;
	}
	default_random_engine generator;
	uniform_int_distribution<int> d(0, vn-1);
	map<edge, bool> table;
	for(int i=0; i<en; i++){
		int u = d(generator);
		int v = d(generator);
		if(u == v){
			i--;
			continue;
		}
		if(u > v)
			swap(u,v);
		if(table[edge(u, v)]){
			i--;
			continue;
		}
		table[edge(u, v)] = true;
		addEdge(u, v);
	}
}

void graph::addEdge(int u, int v){
	edges.push_back(edge(u, v));
	int uidx = idxMap[u];
	int vidx = idxMap[v];
	vertices[uidx].addAdjacent(v);
	vertices[vidx].addAdjacent(u);
}

bool graph::isEmpty(){
	return vertices.empty();
}

int graph::vertices_number(){
	return vertices.size();
}

int graph::edge_number(){
	return edges.size();
}

int graph::getidx(int label){
	return idxMap[label];
}

void graph::dump(){
	cout << "vertex number: " << vertices.size() << endl;
	for(int i=0; i<vertices.size(); i++){
		cout << "vertex label: " << vertices[i].label << endl;
		cout << "adjacent: " << endl;
		for(int j=0; j<vertices[i].adj.size(); j++){
			cout << vertices[i].adj[j] << ", ";
		}
		cout << endl;
	}
}

graph graph::subgraph(set<int> v){
	graph subg(v);
	for(int i=0; i<edges.size(); i++){
		int ulabel = edges[i].first;
		int vlabel = edges[i].second;
		//Check if edge's endpoint belong to new graph
		if(v.count(ulabel) > 0){
			if(v.count(vlabel) > 0){
				subg.addEdge(ulabel, vlabel);
			}
		}
	}
	return subg;
}

graph graph::subgraph(concurrent_unordered_set<int> v){
	graph subg(v);
	for(int i=0; i<edges.size(); i++){
		int ulabel = edges[i].first;
		int vlabel = edges[i].second;
		//Check if edge's endpoint belong to new graph
		if(v.count(ulabel) > 0){
			if(v.count(vlabel) > 0){
				subg.addEdge(ulabel, vlabel);
			}
		}
	}
	return subg;
}

bool graph::verifyMis(set<int> s){
	//Check if s has no mutual adjacent vertices
	//Collect s and s's neighbors
	set<int> v = s;
	for(set<int>::iterator it=s.begin(); it!=s.end(); it++){
		int uidx = idxMap[*it];
		vector<int>::iterator adjIt = (vertices[uidx].adj).begin();
		for(; adjIt!=vertices[uidx].adj.end(); adjIt++){
			//adjacent vertex find
			if(s.count(*adjIt) > 0){
				cout << "Mutual adjacent: " << *it << ", " << *adjIt << endl;
				return false;
			}
			v.insert(*adjIt);
		}
	}
	if(v.size() != vertices.size()){
		cout << "Total vertices unequal: " << v.size() << endl;
		return false;
	}
}