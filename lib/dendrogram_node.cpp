#include "dendrogram_node.h"
#include "stdlib.h"
#include "logger.h"
#include <iostream>
#include <sstream>

static int __DendrogramNode_index;
//static void DendrogramNode_index_reset() { __DendrogramNode_index = 0; }

DendrogramNode::DendrogramNode(NodeType type, Node value) : value(value), type(type), parent(NULL)
{
    internalIndex = __DendrogramNode_index++;
    std::ostringstream oss;
    oss << (type == NODE_INTERNAL ? "INTERNAL" : "LEAF") << internalIndex;
    this->uniqueName = oss.str();
}

DendrogramNode::~DendrogramNode() { }

InternalNode::InternalNode(DendrogramNode *left, DendrogramNode *right) : DendrogramNode(NODE_INTERNAL), left(left), right(right), lastPermutation(PERMUTE_NONE), probability(0.0f), needsUpdate(true)
{
    if (left != NULL) {
        left->parent = this;
    }
    if (right != NULL) {
        right->parent = this;
    }
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
    this->internalIndex = copy->internalIndex;
    this->uniqueName = copy->uniqueName;
}

InternalNode::~InternalNode() { }

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

std::string InternalNode::toDot(Corpus *corpus)
{
    std::ostringstream oss;
    
    oss << "\t" << this->uniqueName << "[label=\"" << this->probability << "\", shape=\"none\"];" << std::endl;
    oss << "\t" << this->uniqueName << " -> " << left->uniqueName << ";" << std::endl;
    oss << "\t" << this->uniqueName << " -> " << right->uniqueName << ";" << std::endl;
    oss << left->toDot(corpus);
    oss << right->toDot(corpus);
    
    return oss.str();
}

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
LeafNode::~LeafNode() { }

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
        sprintf(string,"\t%s [label=\"%u\"]\n",uniqueName.c_str(),value);
    } else {
        sprintf(string,"\t%s [label=\"%s\"]\n",uniqueName.c_str(),corpus->indexToString(value).c_str());
    }
    
    return std::string(string);
}
