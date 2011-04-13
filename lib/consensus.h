//
//  consensus.h
//  hrgiew
//
//  Created by Trevor Fountain on 4/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __CONSENSUS_H__
#define __CONSENSUS_H__

#include <set>
#include <string>
#include "graph.h"
#include "dendrogram.h"
#include "dendrogram_node.h"

class ConsensusNode
{
public:
    std::set<ConsensusNode *> children;
    NodeType type;
    
    ConsensusNode();
    ConsensusNode(std::set<ConsensusNode *> children);
    virtual std::string toString();
};

class ConsensusLeaf : public ConsensusNode
{
public:
    Node value;
    
    ConsensusLeaf(Node value);
    std::string toString();
};

typedef std::set<Dendrogram *> DendrogramSet;
typedef std::set<Node> Cluster;

class Consensus
{
protected:
    std::set<Cluster> getClusters(Dendrogram *dendro);
    std::set<ConsensusNode *>nodes;
    ConsensusNode *root;
public:
    Consensus(DendrogramSet dendrograms, Graph *graph);
    std::string toString();
};

#endif