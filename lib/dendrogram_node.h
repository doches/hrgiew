#include "graph.h"

typedef enum _Permutation_t { PERMUTE_LL, PERMUTE_LR, PERMUTE_RL, PERMUTE_RR, PERMUTE_NONE } Permutation;

typedef enum _NodeType_t { NODE_LEAF, NODE_INTERNAL, NODE_NONE } NodeType;

#define SWAP(_A_,_B_) DendrogramNode *temp = _A_; _A_ = _B_; _B_ = temp;

class DendrogramNode
{
public:
    Node value;
    NodeType type;
    DendrogramNode(NodeType type=NODE_NONE, Node value="");
    virtual std::set<Node> getChildren() = 0;
    virtual void print(int level=0) = 0;
};

class InternalNode : public DendrogramNode
{
protected:
    DendrogramNode *left;
    DendrogramNode *right;
    
    Permutation lastPermutation;
    static Permutation chooseRandomPermutation();
public:
	InternalNode(DendrogramNode *left, DendrogramNode *right);
    InternalNode(InternalNode *other);
    
    void permute(Permutation permutation=PERMUTE_NONE);
    void revert();
    
    DendrogramNode *getLeft();
    DendrogramNode *getRight();
    void setLeft(DendrogramNode *left);
    void setRight(DendrogramNode *right);
    double probability;
    std::set<Node> getChildren();
    void print(int level=0);
};

class LeafNode : public DendrogramNode
{
public:
    LeafNode(Node value);
    std::set<Node> getChildren();
    void print(int level=0);
};