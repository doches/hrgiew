//
//  consensus.cpp
//  hrgiew
//
//  Created by Trevor Fountain on 4/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "consensus.h"
#include <map>
#include <sstream>
#include <iostream>

ConsensusNode::ConsensusNode(std::set<ConsensusNode *>children)
{
    this->children = children;
    this->type = NODE_INTERNAL;
}

ConsensusNode::ConsensusNode()
{
    this->type = NODE_INTERNAL;
}

std::string ConsensusNode::toString(Corpus *corpus)
{
    std::string str = "(";
    for (std::set<ConsensusNode *>::iterator iter=children.begin(); iter!=children.end(); iter++) {
        if (*iter != NULL) {
            str += (*iter)->toString(corpus);
            
            if (++iter != children.end()) {
                str += " ";
            }
            --iter;
        }
    }
    str += ")";
    return str;
}

ConsensusLeaf::ConsensusLeaf(Node value) : ConsensusNode()
{
    this->value = value;
    this->type = NODE_LEAF;
}

std::string ConsensusLeaf::toString(Corpus *corpus)
{
    std::ostringstream oss;
    if (corpus != NULL) {
        oss << corpus->indexToString(this->value);
    } else {
        oss << "<" << this->value << ">";
    }
    return oss.str();
}

bool clusterCompare(Cluster a, Cluster b) {
    if (a.size() != b.size()) {
        return a.size() < b.size();
    }
    
    return a < b;
}

Consensus::Consensus(DendrogramSet dendrograms, Graph *graph, Corpus *corpus)
{
    std::map<Cluster,double,bool(*)(Cluster,Cluster)> clusterWeights(&clusterCompare);
    double totalWeight = 0.0f;
    
    // Compute the weight for each observed cluster
    for (DendrogramSet::iterator iter = dendrograms.begin(); iter != dendrograms.end(); iter++) {
        std::set<Cluster> clusters = getClusters(*iter);
        
        double likelihood = 1.0/(-(*iter)->likelihood());
        for (std::set<Cluster>::iterator citer = clusters.begin(); citer != clusters.end(); citer++) {
            clusterWeights[*citer] += likelihood;
        }
        totalWeight += likelihood;
    }
    
    // Normalize by the sum likelihood of all dendrograms,
    // and find elements with < 50% likelihood
    std::set<Cluster>toDelete;
    for (std::map<Cluster,double>::iterator iter = clusterWeights.begin(); iter != clusterWeights.end(); iter++) {
        iter->second /= totalWeight;
        
        if (iter->second < 0.5) {
            toDelete.insert(iter->first);
        }
    }
    
    // Remove clusters with < 50% likelihood.
    for (std::set<Cluster>::iterator iter=toDelete.begin(); iter != toDelete.end(); iter++) {
        clusterWeights.erase(*iter);
    }
    
    // DEBUG: print retained clusters
//    for (std::map<Cluster,double>::iterator iter = clusterWeights.begin(); iter != clusterWeights.end(); iter++) {
//        std::cout << "[";
//        for (Cluster::iterator word = iter->first.begin(); word != iter->first.end(); word++) {
//            if (corpus == NULL) {
//                std::cout << *word << " ";
//            } else {
//                std::cout << corpus->indexToString(*word) << " ";
//            }
//        }
//        std::cout << "] " << iter->second << "\n";
//    }
    
    // Keep a running map of leaf strings -> highest parent
    std::map<Node,ConsensusNode *>foundIn;
    for (std::set<Node>::iterator iter= graph->nodes.begin(); iter != graph->nodes.end(); iter++) {
        foundIn[*iter] = new ConsensusLeaf(*iter);
    }
    
    for (std::map<Cluster,double>::iterator iter=clusterWeights.begin(); iter != clusterWeights.end(); iter++) {
        std::set<ConsensusNode *>children;
        // Build up a set of consensus nodes, one per child leaf string
        for (Cluster::iterator clusterIter = iter->first.begin(); clusterIter != iter->first.end(); clusterIter++) {
            children.insert(foundIn[*clusterIter]);
        }
        
        // Create a new consensus node
        ConsensusNode *node = new ConsensusNode(children);
        nodes.insert(node);
        root = node;
        
        // Update foundIn map with new node
        for (Cluster::iterator clusterIter = iter->first.begin(); clusterIter != iter->first.end(); clusterIter++) {
            foundIn[*clusterIter] = node;
        }
    }
}

std::string Consensus::toString(Corpus *corpus)
{
    return this->root->toString(corpus);
}

std::string Consensus::toDot(Corpus *corpus)
{
    std::ostringstream dot;
    dot << "graph Consensus {" << std::endl;
    int nodeIndex = 0;
    for (std::set<ConsensusNode *>::iterator iter=nodes.begin(); iter != nodes.end(); iter++) {
        ConsensusNode *node = *iter;
        
        if (node->type == NODE_INTERNAL) {
            node->uid = nodeIndex++;
        }
    }
    for (std::set<ConsensusNode *>::iterator iter=nodes.begin(); iter != nodes.end(); iter++) {
        ConsensusNode *node = *iter;
        dot << "\tINTERNAL" << node->uid << " [label=\"\", shape=point];" << std::endl;
        for(std::set<ConsensusNode *>::iterator childIter=node->children.begin(); childIter != node->children.end(); childIter++) {
            ConsensusNode *child = *childIter;
            dot << "\t" << (node->type == NODE_INTERNAL ? "INTERNAL" : "LEAF") << node->uid << " -- ";
            if (child->type == NODE_INTERNAL) {
                dot << "INTERNAL" << child->uid << ";" << std::endl;
            } else {
                Node value = ((ConsensusLeaf *)child)->value;
                dot << "LEAF" << value << ";" << std::endl;
                dot << "\tLEAF" << value << " [label=\"";
                if (corpus == NULL) {
                    dot << value;
                } else {
                    dot << corpus->indexToString(value);
                } 
                dot << "\"];"<<std::endl;
            }
        }
    }
    
    dot << "}"<<std::endl;
    
    return dot.str();
}

std::set<Cluster> Consensus::getClusters(Dendrogram *dendro)
{
    std::set<Cluster> clusters;
    
    for (NodeList::iterator iter=dendro->nodes.begin(); iter != dendro->nodes.end(); iter++) {
        Cluster cluster = (*iter)->getChildren();
        if (cluster.size() > 1) {
            clusters.insert(cluster);
        }
    }
    
    return clusters;
}