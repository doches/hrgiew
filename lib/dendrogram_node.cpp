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
}

Permutation InternalNode::chooseRandomPermutation()
{
    return (Permutation)(rand()%PERMUTE_NONE);
}

void InternalNode::permute(Permutation permutation)
{
    if (permutation == PERMUTE_NONE) {
        permutation = InternalNode::chooseRandomPermutation();
        lastPermutation = permutation;
    }
    
    switch (permutation) {
        case PERMUTE_LL:
            if (left->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)left)->left,right);
            }
            break;
        case PERMUTE_LR:
            if (left->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)left)->right,right);
            }
            break;
        case PERMUTE_RL:
            if (right->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)right)->left,left);
            }
            break;
        case PERMUTE_RR:
            if (right->type == NODE_INTERNAL) {
                SWAP(((InternalNode *)right)->right,left);
            }
            break;
        default:
            Log::warn("InternalNode","Ignoring invalid permutation type");
            break;
    }
}

void InternalNode::revert()
{
    permute(lastPermutation);
    lastPermutation = PERMUTE_NONE;
}

void InternalNode::print(int level)
{
    for (int i=0; i<level; i++) {
        std::cout << " ";
    }
    printf("(%1.2f)\n",probability);
    left->print(level+1);
    right->print(level+1);
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
    std::set<Node>children;
    
    std::set<Node>leftChildren = left->getChildren();
    for (std::set<Node>::iterator iter = leftChildren.begin(); iter != leftChildren.end(); iter++) {
        children.insert(*iter);
    }
    std::set<Node>rightChildren = right->getChildren();
    for (std::set<Node>::iterator iter = rightChildren.begin(); iter != rightChildren.end(); iter++) {
        children.insert(*iter);
    }
    
    return children;
}

LeafNode::LeafNode(Node value) : DendrogramNode(NODE_LEAF, value) { }

std::set<Node>LeafNode::getChildren()
{
    std::set<Node>children;
    children.insert(value);
    return children;
}

void LeafNode::print(int level)
{
    for (int i=0; i<level; i++) {
        std::cout << " ";
    }
    std::cout << "[<"<<value<<">]" << std::endl;
}