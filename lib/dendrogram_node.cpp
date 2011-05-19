#include "dendrogram_node.h"
#include "stdlib.h"
#include "logger.h"
#include <iostream>

DendrogramNode::DendrogramNode(NodeType type, Node value) : value(value), type(type), parent(NULL) { }

InternalNode::InternalNode(DendrogramNode *left, DendrogramNode *right) : DendrogramNode(NODE_INTERNAL), left(left), right(right), lastPermutation(PERMUTE_NONE), probability(0.0f), needsUpdate(true)
{
    left->parent = this;
    right->parent = this;
}

InternalNode::InternalNode(InternalNode *copy)
{
    this->lastPermutation = PERMUTE_NONE;
    this->probability = copy->probability;
    this->left = copy->left;
    this->right = copy->right;
    this->type = NODE_INTERNAL;
    this->parent = copy->parent;
    this->needsUpdate = copy->needsUpdate;
}

Permutation InternalNode::chooseRandomPermutation()
{
    return (Permutation)(rand()%PERMUTE_NONE);
}

std::set<InternalNode *> InternalNode::permute(Permutation permutation)
{
    std::set<InternalNode *> modified;
    
    if (permutation == PERMUTE_NONE) {
        permutation = InternalNode::chooseRandomPermutation();
        lastPermutation = permutation;
    }
    
    childCache.clear();
    
    switch (permutation) {
        case PERMUTE_LL:
            if (left->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)left)->left,right);
                ((InternalNode *)left)->resetChildCache();
                
                left->parent = this;
                right->parent = this;
                ((InternalNode *)left)->left->parent = (InternalNode *)left;
                this->needsUpdate = true;
                ((InternalNode *)left)->needsUpdate = true;
                
                modified.insert((InternalNode *)left);
            }
            break;
        case PERMUTE_LR:
            if (left->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)left)->right,right);
                ((InternalNode *)left)->resetChildCache();
                
                left->parent = this;
                right->parent = this;
                ((InternalNode *)left)->right->parent = (InternalNode *)left;
                this->needsUpdate = true;
                ((InternalNode *)left)->needsUpdate = true;
                
                modified.insert((InternalNode *)left);
            }
            break;
        case PERMUTE_RL:
            if (right->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)right)->left,left);
                ((InternalNode *)right)->resetChildCache();
                
                left->parent = this;
                right->parent = this;
                ((InternalNode *)right)->left->parent = (InternalNode *)right;
                this->needsUpdate = true;
                ((InternalNode *)right)->needsUpdate = true;
                
                modified.insert((InternalNode *)right);
            }
            break;
        case PERMUTE_RR:
            if (right->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)right)->right,left);
                ((InternalNode *)right)->resetChildCache();
                
                left->parent = this;
                right->parent = this;
                ((InternalNode *)right)->right->parent = (InternalNode *)right;
                this->needsUpdate = true;
                ((InternalNode *)right)->needsUpdate = true;
                
                modified.insert((InternalNode *)right);
            }
            break;
        default:
            Log::warn("InternalNode","Ignoring invalid permutation type");
            exit(1);
            break;
    }
    modified.insert((InternalNode *)this);
    return modified;
}

std::set<InternalNode *> InternalNode::revert()
{
    std::set<InternalNode *> modified = permute(lastPermutation);
    lastPermutation = PERMUTE_NONE;
    return modified;
}

void InternalNode::resetChildCache()
{
    childCache.clear();
}

void InternalNode::print(int level, Corpus *corpus)
{
    for (int i=0; i<level; i++) {
        std::cout << " ";
    }
    printf("(%1.2f)\n",probability);
    left->print(level+1,corpus);
    right->print(level+1,corpus);
}

std::string InternalNode::toString(Corpus *corpus)
{
    char string[80];
    sprintf(string,"%p (%1.10f):\t%p\t%p\n",this,probability,left,right);
    
    return std::string(string) + left->toString(corpus) + right->toString(corpus);
}

static int __InternalNode_toDot_index;
std::string InternalNode::toDot(Corpus *corpus)
{
    char string[80];
    sprintf(string,"\tINTERNAL%d [label=\"%1.4f\", shape=\"none\"];\n",__InternalNode_toDot_index,probability);
    
    char links[100];
    sprintf(links,"\tINTERNAL%d -> %s%d;\n\tINTERNAL%d -> %s%d;\n",
    						  __InternalNode_toDot_index,
    						  (left->type == NODE_INTERNAL ? "INTERNAL" : "LEAF"),0,
    						  __InternalNode_toDot_index,
    						  (right->type == NODE_INTERNAL ? "INTERNAL" : "LEAF"),0);
    
    __InternalNode_toDot_index++;
    return std::string(string) + std::string(links) + 
           left->toDot(corpus) + right->toDot(corpus);
}

static void InternalNode_toDot_reset() { __InternalNode_toDot_index = 0; }

DendrogramNode *InternalNode::getLeft()
{
    return this->left;
}

DendrogramNode *InternalNode::getRight()
{
    return this->right;
}

void InternalNode::setLeft(DendrogramNode *left)
{
    this->left = left;
    left->parent = this;
}

void InternalNode::setRight(DendrogramNode *right)
{
    this->right = right;
    right->parent = this;
}

std::set<Node>InternalNode::getChildren()
{
    if (childCache.size() <= 0) {
        std::set<Node>leftChildren = left->getChildren();
        for (std::set<Node>::iterator iter = leftChildren.begin(); iter != leftChildren.end(); iter++) {
            childCache.insert(*iter);
        }
        std::set<Node>rightChildren = right->getChildren();
        for (std::set<Node>::iterator iter = rightChildren.begin(); iter != rightChildren.end(); iter++) {
            childCache.insert(*iter);
        }
    }
    
    return childCache;
}

LeafNode::LeafNode(Node value) : DendrogramNode(NODE_LEAF, value) { }

std::set<Node>LeafNode::getChildren()
{
    std::set<Node>children;
    children.insert(value);
    return children;
}

void LeafNode::print(int level, Corpus *corpus)
{
    for (int i=0; i<level; i++) {
        std::cout << " ";
    }
    if (corpus == NULL) {
        std::cout << "[<"<<value<<">]" << std::endl;
    } else {
        std::cout << "[<"<<corpus->indexToString(value)<<">]" << std::endl;
    }
}

std::string LeafNode::toString(Corpus *corpus)
{
    char string[100];
    
    if (corpus == NULL) {
        sprintf(string,"%p: [%u]\n",this,value);
    } else {
        sprintf(string,"%p: [%s]\n",this,corpus->indexToString(value).c_str());
    }
    
    return std::string(string);
}

std::string LeafNode::toDot(Corpus *corpus)
{
    char string[100];
    
    if (corpus == NULL) {
        sprintf(string,"\t%p [label=\"%u\"]\n",this,value);
    } else {
        sprintf(string,"\t%p [label=\"%s\"]\n",this,corpus->indexToString(value).c_str());
    }
    
    return std::string(string);
}
