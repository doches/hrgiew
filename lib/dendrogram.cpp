#include "dendrogram.h"
#include "logger.h"
#include <set>
#include <cmath>
#include <map>

Dendrogram::Dendrogram(Dendrogram *other)
{
    this->graph = other->graph;
    
    std::map<DendrogramNode *,DendrogramNode *> cloneMap;
    
    for (std::set<InternalNode *>::iterator iter=other->nodes.begin(); iter != other->nodes.end(); iter++) {
        InternalNode *clone = new InternalNode(*iter);
        cloneMap.insert( std::pair<InternalNode *,InternalNode *>(*iter,clone) );
        this->nodes.insert(clone);
    }
    
    this->root = cloneMap[other->root];
    
    for (std::set<InternalNode *>::iterator iter=nodes.begin(); iter != nodes.end(); iter++) {
        InternalNode *node = *iter;
        if (node->getLeft()->type == NODE_INTERNAL) {
            node->setLeft(cloneMap[node->getLeft()]);
        }
        if (node->getRight()->type == NODE_INTERNAL) {
            node->setRight(cloneMap[node->getRight()]);
        }
    }
}

Dendrogram::Dendrogram(Graph *graph) : graph(graph)
{
    std::set<DendrogramNode *>nodesBuild;
    // Initialize builder set to contain leaf nodes.
    for (std::set<Node>::iterator iter = graph->nodes.begin(); iter != graph->nodes.end(); iter++) {
        LeafNode *leaf = new LeafNode(*iter);
        nodesBuild.insert(leaf);
    }
    
    // Incrementally pair up nodes in the builder set, until there's only one left (the root).
    while (nodesBuild.size() > 1) {
        InternalNode *parent = new InternalNode(*(nodesBuild.begin()),*(nodesBuild.rbegin()));
        nodesBuild.erase(parent->getLeft());
        nodesBuild.erase(parent->getRight());
        nodesBuild.insert(parent);
        nodes.insert(parent);
    }
    
    this->root = *(nodesBuild.begin());
}

// Update probabilities for each internal node
void Dendrogram::updateProbabilities()
{ 
    for (std::set<InternalNode *>::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
        InternalNode *node = *iter;
        if (node->type == NODE_INTERNAL) {
            std::set<Node> leftChildren = node->getLeft()->getChildren();
            std::set<Node> rightChildren = node->getRight()->getChildren();
            node->probability = graph->linksBetween(leftChildren,rightChildren) / (double)(leftChildren.size() * rightChildren.size());
        }
    }
}

void Dendrogram::print()
{
    this->root->print();
}

double Dendrogram::sample()
{
    InternalNode *node = NULL;
    int randomIndex = rand()%nodes.size();
    for (std::set<InternalNode *>::iterator iter=nodes.begin(); iter != nodes.end(); iter++) {
        randomIndex--;
        
        if (randomIndex < 0) {
            node = *iter;
            break;
        }
    }
    
    double oldLikelihood = this->likelihood();
    node->permute();
    double newLikelihood = this->likelihood();
    if (newLikelihood >= oldLikelihood) {
        return newLikelihood;
    }
    else {
        double acceptProbability = (rand()%10000)/10000.0; // TODO Be more random!
        if (acceptProbability > newLikelihood / oldLikelihood) {
            node->revert();
            newLikelihood = oldLikelihood;
        }
    }
    
    return newLikelihood;
}

double Dendrogram::likelihood()
{
    this->updateProbabilities();
    
    double product = 1.0f;
    for (std::set<InternalNode *>::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
        InternalNode *node = *iter;
        
        std::set<Node> leftChildren = node->getLeft()->getChildren();
        std::set<Node> rightChildren = node->getRight()->getChildren();
        double eNode = (double)(graph->linksBetween(leftChildren,rightChildren));
        
        product *= pow(node->probability, eNode) * pow(1-node->probability,leftChildren.size()*rightChildren.size() - eNode);
    }
    
    return product;
}

DendrogramNode *Dendrogram::getRoot()
{
    return this->root;
}