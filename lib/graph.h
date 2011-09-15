#include <set>
#include <map>
#include <string>

#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef unsigned int Node;
typedef std::pair<unsigned int, unsigned int> Key;
typedef std::set<Node> NodeSet;

typedef std::pair<NodeSet,NodeSet> CacheKey;

class Edge
{
public:
    Node left;
    Node right;

    double weight;

    Edge(Node a, Node b, double weight=1.0);
    std::string toString();
};

class Graph
{
protected:
    bool loadFromPairs(std::string filename);
    bool loadFromWeights(std::string filename);
    bool valid;
    
    
    std::map<CacheKey,double> linksCache;
	
public:
    std::set<Node>nodes;
    std::map<Key, Edge *>edges;
    double threshold;

    Graph(const std::string filename);
    Graph();
    std::string toString();
    bool isValid();
    double linksBetween(std::set<Node> a, std::set<Node> b);
    void setWeight(Node a, Node b, double weight);

    void addNode(Node node);
    void addEdge(Node a, Node b, double weight=1.0);
};

#endif
