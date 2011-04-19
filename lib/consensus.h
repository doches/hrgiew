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
#include "corpus.h"

class ConsensusNode
{
public:
    std::set<ConsensusNode *> children;
    NodeType type;
    unsigned int uid;
    
    ConsensusNode();
    ConsensusNode(std::set<ConsensusNode *> children);
    virtual std::string toString(Corpus *corpus=NULL);
};

class ConsensusLeaf : public ConsensusNode
{
public:
    Node value;
    
    ConsensusLeaf(Node value);
    std::string toString(Corpus *corpus=NULL);
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
    Consensus(DendrogramSet dendrograms, Graph *graph, Corpus *corpus=NULL);
    std::string toString(Corpus *corpus=NULL);
    std::string toDot(Corpus *corpus=NULL);
};

#endif