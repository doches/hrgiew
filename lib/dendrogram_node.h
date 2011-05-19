#ifndef __DENDROGRAM_NODE_H__
#define __DENDROGRAM_NODE_H__

#include "graph.h"
#include "corpus.h"

typedef enum _Permutation_t { PERMUTE_LL, PERMUTE_LR, PERMUTE_RL, PERMUTE_RR, PERMUTE_NONE } Permutation;

typedef enum _NodeType_t { NODE_LEAF, NODE_INTERNAL, NODE_NONE } NodeType;

#define SWAP(_A_,_B_) DendrogramNode *temp = _A_; _A_ = _B_; _B_ = temp; 

class InternalNode;

class DendrogramNode
{
public:
    Node value;
    NodeType type;
    DendrogramNode(NodeType type=NODE_NONE, Node value=0);
    virtual std::set<Node> getChildren() = 0;
    virtual void print(int level=0, Corpus *corpus=NULL) = 0;
    virtual std::string toString(Corpus *corpus=NULL) = 0;
    
    InternalNode *parent;
};

class InternalNode : public DendrogramNode
{
protected:
    DendrogramNode *left;
    DendrogramNode *right;
    
    Permutation lastPermutation;
    static Permutation chooseRandomPermutation();
    std::set<Node> childCache;
public:
	InternalNode(DendrogramNode *left, DendrogramNode *right);
    InternalNode(InternalNode *other);
    
    std::set<InternalNode *> permute(Permutation permutation=PERMUTE_NONE);
    std::set<InternalNode *> revert();
    
    void resetChildCache();
    DendrogramNode *getLeft();
    DendrogramNode *getRight();
    void setLeft(DendrogramNode *left);
    void setRight(DendrogramNode *right);
    double probability;
    std::set<Node> getChildren();
    void print(int level=0, Corpus *corpus=NULL);
    std::string toString(Corpus *corpus=NULL);
    
    bool needsUpdate;
};

class LeafNode : public DendrogramNode
{
public:
    LeafNode(Node value);
    std::set<Node> getChildren();
    void print(int level=0, Corpus *corpus=NULL);
    std::string toString(Corpus *corpus=NULL);
};

#endif