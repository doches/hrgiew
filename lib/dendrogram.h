#ifndef __DENDROGRAM_H__
#define __DENDROGRAM_H__

#include "graph.h"
#include "dendrogram_node.h"
#include "corpus.h"
#include <set>
#include <vector>

typedef std::vector<InternalNode *> NodeList;

class Dendrogram
{
protected:
	DendrogramNode *root;
    Graph *graph;
    std::set<LeafNode *>leaves;
    void updateProbabilities();
    InternalNode *findParent(Node node, InternalNode *subtree);
    void validateCopy(DendrogramNode *node);
public:
    NodeList nodes;
    
	Dendrogram(Graph *graph);
    Dendrogram(Dendrogram *dendrogram);
    bool sample();
    double likelihood();
    DendrogramNode *getRoot();
    void print(Corpus *corpus=NULL);
    void addLeaf(Node leaf, Node hint=0);
};

#endif
