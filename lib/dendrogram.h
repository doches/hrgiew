#ifndef __DENDROGRAM_H__
#define __DENDROGRAM_H__

#include "graph.h"
#include "dendrogram_node.h"
#include "corpus.h"
#include <set>
#include <vector>
#include <string>

typedef std::vector<InternalNode *> NodeList;

/// @brief Hackish structure used while loading a dendrogram from a graph + file
///
/// Don't use elsewhere!
typedef struct __DendrogramLoaderNode_t
{
    bool internal;
    
    Node value;

    DendrogramNode *left;
    DendrogramNode *right;
} DendrogramLoaderNode;

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
    std::set<InternalNode *> modified;
    NodeList nodes;
    
    Dendrogram(Graph *graph);
    Dendrogram(Dendrogram *dendrogram);
    Dendrogram(Graph *graph, const char *filename);
    bool sample();
    double likelihood();
    DendrogramNode *getRoot();
    void print(Corpus *corpus=NULL);
    void addLeaf(Node leaf, Node hint=0);
    std::string toString(Corpus *corpus=NULL);
    std::string toDot(Corpus *corpus=NULL);
};

#endif
