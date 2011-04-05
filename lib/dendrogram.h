#ifndef __DENDROGRAM_H__
#define __DENDROGRAM_H__

#include "graph.h"
#include "dendrogram_node.h"
#include <set>

class Dendrogram
{
protected:
	DendrogramNode *root;
    Graph *graph;
    std::set<InternalNode *>nodes;
    void updateProbabilities();
public:
	Dendrogram(Graph *graph);
    double sample();
    double likelihood();
    DendrogramNode *getRoot();
    void print();
};

#endif
