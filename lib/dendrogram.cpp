#include <stdlib.h>
#include "dendrogram.h"
#include "logger.h"
#include <set>
#include <cmath>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>

Dendrogram::Dendrogram(Dendrogram *other)
{
    this->graph = other->graph;
    
    std::map<DendrogramNode *,DendrogramNode *> cloneMap;
    
    this->leaves = other->leaves;
    
    for (NodeList::iterator iter=other->nodes.begin(); iter != other->nodes.end(); iter++) {
        InternalNode *clone = new InternalNode(*iter);
        cloneMap.insert(std::pair<InternalNode *,InternalNode *>(*iter,clone) );
        this->nodes.push_back(clone);
        modified.insert(clone);
    }
    
    for (NodeList::iterator iter=nodes.begin(); iter != nodes.end(); iter++) {
        InternalNode *node = *iter;
        if (node->getLeft()->type == NODE_INTERNAL) {
            node->setLeft(cloneMap[node->getLeft()]);
        }
        if (node->getRight()->type == NODE_INTERNAL) {
            node->setRight(cloneMap[node->getRight()]);
        }
        if (node->parent != NULL) {
            node->parent = (InternalNode *)cloneMap[node->parent];
        }
    }
    
    this->root = cloneMap[((InternalNode *)other->root)];
    
    validateCopy(root);
}

void Dendrogram::validateCopy(DendrogramNode *node)
{
    if (node->type == NODE_INTERNAL) {
        if (std::find(nodes.begin(), nodes.end(), (InternalNode *)node)==nodes.end()) {
            Log::warn("Dendrogram","Failed copy validation (internal)");
            node->print(0);
            std::cout << node << std::endl;
            exit(1);
        } else {
            validateCopy(((InternalNode *)node)->getLeft());
            validateCopy(((InternalNode *)node)->getRight());
        }
    } else {
        if (leaves.find((LeafNode *)node) == leaves.end()) {
            Log::warn("Dendrogram","Failed copy validation (leaf)");
            node->print(0);
            exit(1);
        }
    }
}

Dendrogram::Dendrogram(Graph *graph, const char *filename) : graph(graph)
{
    // 0xF0000000 -> DendrogramNode
    std::map<long long,DendrogramNode *> nodeMap;
    // Node value -> LeafNode
    std::map<Node,LeafNode *> leafNodeMap;
    
    // Load graph, converting graph nodes into LeafNodes
    for (std::set<Node>::iterator iter = graph->nodes.begin(); iter != graph->nodes.end(); iter++) {
        LeafNode *leaf = new LeafNode(*iter);
        leaves.insert(leaf);
        
        leafNodeMap[leaf->value] = leaf;
    }
    
    // Load dendrogram
    this->root = NULL;
    std::ifstream fin(filename, std::ifstream::in);
    std::map<InternalNode *,std::pair<long long,long long> > fixup;
    char line[255];
    while (fin.good()) {
        fin.getline(line,255);
        
        if (strlen(line) > 1) {
            char *word = strtok(line," ");
            if (word[strlen(word)-1] == ':') {
                // Leaf node
                //
                // 0x100102fb0: [35]
                
                char ptr[15];
                strcpy(ptr,word);
                ptr[strlen(ptr)-1] = '\0';
                
                char id[10];
                word = strtok(NULL," ");
                strcpy(id,word+1);
                id[strlen(id)-1] = '\0';
                unsigned int value = (unsigned int)atoi(id);
                
                LeafNode *leaf = new LeafNode(value);
                leaves.insert(leaf);
                
                nodeMap[strtol(ptr,NULL,16)] = leaf;
            } else {
                // Internal node
                // 
                // 0x100101c30 (0.0000000000):	0x100102fb0	0x100102fe0
                
                char ptr[15];
                strcpy(ptr,word);
                
                char prob[15];
                word = strtok(NULL," \t");
                strcpy(prob,word+1);
                prob[strlen(prob)-2] = '\0';
                double probability = atof(prob);
                
                char left[15],right[15];
                strcpy(left,strtok(NULL," \t"));
                strcpy(right,strtok(NULL," \t\n"));
                
                InternalNode *node = new InternalNode(NULL,NULL);
                node->probability = probability;
                node->needsUpdate = true;
                nodes.push_back(node);
                
                nodeMap[strtol(ptr,NULL,16)] = node;
                
                fixup[node] = std::pair<long long, long long>(strtol(left,NULL,16),strtol(right,NULL,16));
                
                // Is this the root?
                if (root == NULL) {
                    this->root = node;
                }
            }
        }
    }
    
    for (std::map<InternalNode *,std::pair<long long,long long> >::iterator iter = fixup.begin(); iter != fixup.end(); iter++) {
        InternalNode *node = (InternalNode *)iter->first;
        std::pair<long long,long long> lr_keys = iter->second;
        
        node->setLeft(nodeMap[lr_keys.first]);
        node->setRight(nodeMap[lr_keys.second]);
    }
}

Dendrogram::Dendrogram(Graph *graph) : graph(graph)
{
    std::set<DendrogramNode *>nodesBuild;
    
    // Initialize builder set to contain leaf nodes.
    for (std::set<Node>::iterator iter = graph->nodes.begin(); iter != graph->nodes.end(); iter++) {
        LeafNode *leaf = new LeafNode(*iter);
        nodesBuild.insert(leaf);
        leaves.insert(leaf);
    }
    
    // Incrementally pair up nodes in the builder set, until there's only one left (the root).
    while (nodesBuild.size() > 1) {
        InternalNode *parent = new InternalNode(*(nodesBuild.begin()),*(nodesBuild.rbegin()));
        nodesBuild.erase(parent->getLeft());
        nodesBuild.erase(parent->getRight());
        nodesBuild.insert(parent);
        nodes.push_back(parent);
        modified.insert(parent);
    }
    
    this->root = *(nodesBuild.begin());
    if (this->root==NULL && nodes.size() > 0) {
        Log::warn("Dendrogram","root is NULL after graph constructor");
        exit(1);
    }
}

// Update probabilities for each internal node
void Dendrogram::updateProbabilities()
{
    for (std::set<InternalNode *>::iterator iter=modified.begin(); iter!=modified.end(); iter++) {
        InternalNode *node = *iter;
        if (node->type == NODE_INTERNAL && ((InternalNode *)node)->needsUpdate) {
            std::set<Node> leftChildren = node->getLeft()->getChildren();
            std::set<Node> rightChildren = node->getRight()->getChildren();
            node->probability = graph->linksBetween(leftChildren,rightChildren) / (double)(leftChildren.size() * rightChildren.size());
            ((InternalNode *)node)->needsUpdate = false;
        }
    }
    modified.clear();
}

void Dendrogram::print(Corpus *corpus)
{
    this->root->print(0,corpus);
}

std::string Dendrogram::toString(Corpus *corpus)
{
    return this->root->toString(corpus);
}

std::string Dendrogram::toDot(Corpus *corpus)
{
    std::ostringstream oss;
    oss << "digraph {" << std::endl;
    oss << root->toDot(corpus) << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

bool Dendrogram::sample()
{
    if (root == NULL) {
        return false;
    }
    InternalNode *node = NULL;
    int randomIndex = rand()%nodes.size();
    for (NodeList::iterator iter=nodes.begin(); iter != nodes.end(); iter++) {
        randomIndex--;
        
        if (randomIndex < 0) {
            node = *iter;
            break;
        }
    }
    
    double oldLikelihood = this->likelihood();
    std::set<InternalNode *> permuted = node->permute();
    for (std::set<InternalNode *>::iterator iter = permuted.begin(); iter != permuted.end(); iter++) {
        modified.insert(*iter);
    }
    double newLikelihood = this->likelihood();
    if (newLikelihood >= oldLikelihood) {
        return true;
    }
    else {
        double acceptProbability = (rand()%10000)/10000.0; // TODO Be more random!
        double doAccept = exp(newLikelihood-oldLikelihood);
        if (acceptProbability > doAccept) {
            permuted = node->revert();
            for (std::set<InternalNode *>::iterator iter = permuted.begin(); iter != permuted.end(); iter++) {
                modified.insert(*iter);
            }
            newLikelihood = oldLikelihood;
        }
    }
    
    return false;
}

void Dendrogram::addLeaf(Node leaf, Node hint)
{
    LeafNode *leafNode = new LeafNode(leaf);
    // If we don't have a root node yet, decide if we need to create one.
    if (root == NULL) {
        // Create the new leaf and store it anyway
        leaves.insert(leafNode);
        // Do we have at least two leaves?
        if (leaves.size() == 2) {
            // Create a root node with them as children
            LeafNode *a=NULL,*b=NULL;
            for (std::set<LeafNode *>::iterator leafIter = leaves.begin(); leafIter != leaves.end(); leafIter++) {
                if (a==NULL) {
                    a = *leafIter;
                } else {
                    b = *leafIter;
                }
            }
            root = new InternalNode(a,b);
            nodes.push_back((InternalNode *)root);
        }
        
        return;
    }
    
    // Find the candidate's parent
    InternalNode *parent = findParent(hint,(InternalNode *)root);
    
    // Add in the new node
    InternalNode *subparent;
    if (parent->getLeft()->value == hint) {
        subparent = new InternalNode(parent->getLeft(),leafNode);
        parent->setLeft(subparent);
    } else {
        subparent = new InternalNode(parent->getRight(),leafNode);
        parent->setRight(subparent);
    }
    
    leaves.insert(leafNode);
    nodes.push_back(subparent);
    modified.insert(parent);
    modified.insert(subparent);
}

InternalNode *Dendrogram::findParent(Node node, InternalNode *subtree)
{
    if (subtree->getLeft()->value == node || subtree->getRight()->value == node) {
        return subtree;
    }
    
    std::set<Node>leftChildren = subtree->getLeft()->getChildren();
    if (leftChildren.find(node) != leftChildren.end()) {
        return findParent(node,(InternalNode *)subtree->getLeft());
    } else {
        return findParent(node,(InternalNode *)subtree->getRight());
    }
}

double Dendrogram::likelihood()
{
    double product = 0.0f;
    this->updateProbabilities();
    
    for (NodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
        InternalNode *node = *iter;
        
        std::set<Node> leftChildren = node->getLeft()->getChildren();
        std::set<Node> rightChildren = node->getRight()->getChildren();
        double eNode = (double)(graph->linksBetween(leftChildren,rightChildren));
        
        double p = pow(node->probability, eNode) * pow(1-node->probability,leftChildren.size()*rightChildren.size() - eNode);
        product += log(p);
    }
    
    return product;
}

DendrogramNode *Dendrogram::getRoot()
{
    return this->root;
}
