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
    std::set<LeafNode *>leaves;
    void updateProbabilities();
    InternalNode *findParent(Node node, InternalNode *subtree);
public:
    std::set<InternalNode *>nodes;
    
	Dendrogram(Graph *graph);
    Dendrogram(Dendrogram *dendrogram);
    double sample();
    double likelihood();
    DendrogramNode *getRoot();
    void print();
    void addLeaf(Node leaf, Node hint="");
};

#endif
