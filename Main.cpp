#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <ppl.h>
#include <concurrent_unordered_set.h>
#include "graph.h"
using namespace std;
using namespace concurrency;

//NAIVE IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////
//typedef set<int> vertex;
//
//void dumpGraph(vector<vertex> &graph){
//	for(int i=0; i<graph.size(); i++){
//		cout << "vertex id: " << i << endl;
//		cout << "adjacent list: " ;
//		for(vertex::iterator it=graph[i].begin(); it!=graph[i].end(); it++){
//			cout << *it << " " ;
//		}
//		cout << endl;
//	}
//}
//
//int main(){
//	//read graph
//	ifstream gfile("graph1.txt");
//	int vnum, adjnum, adj;
//	gfile >> vnum;
//	vector<vertex> graph(vnum);
//	set<int> existVertex;
//	for(int i=0; i<vnum; i++){
//		existVertex.insert(i);
//		gfile >> adjnum;
//		for(int j=0; j<adjnum; j++){
//			gfile >> adj;
//			graph[i].insert(adj);
//		}
//	}
//	gfile.close();
//
//	set<int> independentSet;
//	while(!existVertex.empty()){
//		int v = *existVertex.begin();
//		independentSet.insert(v);
//		existVertex.erase(v);
//		cout << "choose: " << v << endl;
//		cout << "erase: " ;
//		for(vertex::iterator it=graph[v].begin(); it!=graph[v].end(); it++){
//			existVertex.erase(*it);
//			cout << *it << " " ;
//		}
//		cout << endl;
//	}
//	return 0;
//}

void mis_naive(graph &g){
	graph subg = g;
	set<int> independentSet;
	clock_t t1, t2;
	t1 = clock();
	double vtime, etime, otime, utime;
	vtime = etime = otime = utime = 0;
	while(!subg.isEmpty()){
		set<int> vset;
		//Select
		//ALG Vertex
		clock_t t3, t4;
		t3 = clock();
		srand(time(NULL));
		vector<int> vrand(subg.vertices_number());
		for(int i=0; i<vrand.size(); i++){
			vrand[i] = rand();
			vset.insert(subg.vertices[i].label);
		}
		t4 = clock();
		vtime += (double)(t4-t3)/CLOCKS_PER_SEC;

		//ALG Edge
		t3 = clock();
		set<int> iset = vset;
		for(vector<edge>::iterator it=subg.edges.begin(); it!=subg.edges.end(); it++){
			int uidx = subg.getidx(it->first);
			int vidx = subg.getidx(it->second);
			if(vrand[uidx] >= vrand[vidx])
				iset.erase(it->first);
			else
				iset.erase(it->second);
		}
		t4 = clock();
		etime += (double)(t4-t3)/CLOCKS_PER_SEC;

		//Union I, I'
		t3 = clock();
		clock_t t5, t6;
		t5 = clock();
		independentSet.insert(iset.begin(), iset.end());
		//Union I', neigbor(I')
		set<int> yset = iset;
		for(set<int>::iterator it=iset.begin(); it!=iset.end(); it++){
			int idx = subg.getidx(*it);
			yset.insert(subg.vertices[idx].adj.begin(), subg.vertices[idx].adj.end());
		}
		t6 = clock();
		utime += (double)(t6-t5)/CLOCKS_PER_SEC;
		//Update subgraph
		//vset.erase(yset.begin(), yset.end());
		for(set<int>::iterator it=yset.begin(); it!=yset.end(); it++)
			vset.erase(*it);
		subg = subg.subgraph(vset);
		t4 = clock();
		otime += (double)(t4-t3)/CLOCKS_PER_SEC;
	}
	t2 = clock();
	cout << "vtime: " << vtime << endl;
	cout << "etime: " << etime << endl;
	cout << "otime: " << otime << endl;
	cout << "utime: " << utime << endl;
	cout << "Time: " << (double)(t2-t1)/CLOCKS_PER_SEC << endl;
	if(g.verifyMis(independentSet))
		cout << "Verified" << endl;
	else 
		cout << "Failed Verified" << endl;

#ifdef DEBUG_MSG
	for(set<int>::iterator it=independentSet.begin(); it!=independentSet.end(); it++)
		cout << *it << endl;
#endif
}

void mis_openmp(graph g){
	graph subg = g;
	set<int> independentSet;
	clock_t t1, t2;
	t1 = clock();
	double vtime, etime, otime, utime;
	vtime = etime = otime = utime = 0;
	while(!subg.isEmpty()){
		set<int> vset;
		//SELECT
		//ALG Vertex
		clock_t t3, t4;
		t3 = clock();
		srand(time(NULL));
		vector<int> vrand(subg.vertices_number());
		vector<bool> iset(g.vertices_number(), false);

//#pragma omp parallel for
		for(int i=0; i<vrand.size(); i++){
			vrand[i] = rand();
			vset.insert(subg.vertices[i].label);
			iset[subg.vertices[i].label] = true;
		}
		
		t4 = clock();
		vtime += (double)(t4-t3)/CLOCKS_PER_SEC;

		//ALG Edge
		t3 = clock();
		int en = subg.edge_number();

#pragma omp parallel for
		for(int i=0; i<en; i++){
			int u = subg.edges[i].first;
			int v = subg.edges[i].second;
			int uidx = subg.getidx(u);
			int vidx = subg.getidx(v);
			if(vrand[uidx] >= vrand[vidx])
				iset[u] = false;
			else
				iset[v] = false;
		}

		t4=clock();
		etime += (double)(t4-t3)/CLOCKS_PER_SEC;

		//Union I, I'
		t3 = clock();
		clock_t t5, t6;
		t5 = clock();
		//concurrent_unordered_set<int> yset;
		set<int> yset;
//#pragma omp parallel for
		//for(int i=0; i<iset.size(); i++){
		for(int j=0; j<subg.vertices_number(); j++){
			//int i=ilist[j];
			int i = subg.vertices[j].label;
			if(iset[i]){
				independentSet.insert(i);
				yset.insert(i);
				int idx = subg.getidx(i);
				yset.insert(subg.vertices[idx].adj.begin(), subg.vertices[idx].adj.end());
			}
		}
		t6 = clock();
		utime += (double)(t6-t5)/CLOCKS_PER_SEC; 
		//Union I', neighbor(I')
		//concurrent_unordered_set<int> yset;
//#pragma omp parallel for
//		for(int i=0; i<iset.size(); i++){
//			if(iset[i]){
//				yset.insert(i);
//				int idx = subg.getidx(i);
//				yset.insert(subg.vertices[idx].adj.begin(), subg.vertices[idx].adj.end());
//			}
//		}


		//Update subgraph
		//concurrent_unordered_set<int>::iterator it;
		set<int>::iterator it;
		for(it=yset.begin(); it!=yset.end(); it++){
			vset.erase(*it);
		}
		//set<int> tmpset(vset.begin(), vset.end());
		subg = subg.subgraph(vset);
		t4 = clock();
		otime += (double)(t4-t3)/CLOCKS_PER_SEC;
	}
	t2 = clock();
	cout << "vtime: " << vtime << endl;
	cout << "etime: " << etime << endl;
	cout << "otime: " << otime << endl;
	cout << "utime: " << utime << endl;
	cout << "Time: " << (double)(t2-t1)/CLOCKS_PER_SEC << endl;
	set<int> iset(independentSet.begin(), independentSet.end());
	if(g.verifyMis(iset))
		cout << "Verified" << endl;
	else 
		cout << "Failed Verified" << endl;

#ifdef DEBUG_MSG
	for(set<int>::iterator it=iset.begin(); it!=iset.end(); it++)
		cout << *it << endl;
#endif 
}

int main(){
	//graph g("com-dblp.ungraph.txt", READ_ADJ);
	graph g("Email-Enron.txt", READ_ADJ);
	//graph g("facebook_combined.txt", READ_ADJ);
	mis_naive(g);
	mis_openmp(g);
	return 0;
}