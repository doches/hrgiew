#include "dendrogram_node.h"
#include "stdlib.h"
#include "logger.h"
#include <iostream>

DendrogramNode::DendrogramNode(NodeType type, Node value) : value(value), type(type) { }

InternalNode::InternalNode(DendrogramNode *left, DendrogramNode *right) : DendrogramNode(NODE_INTERNAL), left(left), right(right), lastPermutation(PERMUTE_NONE), probability(0.0f) { }

InternalNode::InternalNode(InternalNode *copy)
{
    this->lastPermutation = PERMUTE_NONE;
    this->probability = copy->probability;
    this->left = copy->left;
    this->right = copy->right;
    this->type = NODE_INTERNAL;
}

Permutation InternalNode::chooseRandomPermutation()
{
    return (Permutation)(rand()%PERMUTE_NONE);
}

bool InternalNode::permute(Permutation permutation)
{
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
                return true;
            }
            break;
        case PERMUTE_LR:
            if (left->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)left)->right,right);
                ((InternalNode *)left)->resetChildCache();
                return true;
            }
            break;
        case PERMUTE_RL:
            if (right->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)right)->left,left);
                ((InternalNode *)right)->resetChildCache();
                return true;
            }
            break;
        case PERMUTE_RR:
            if (right->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)right)->right,left);
                ((InternalNode *)right)->resetChildCache();
                
                return true;
            }
            break;
        default:
            Log::warn("InternalNode","Ignoring invalid permutation type");
            exit(1);
            break;
    }
    return false;
}

void InternalNode::revert()
{
    permute(lastPermutation);
    lastPermutation = PERMUTE_NONE;
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
}

void InternalNode::setRight(DendrogramNode *right)
{
    this->right = right;
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