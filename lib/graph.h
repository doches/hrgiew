#include <set>
#include <map>
#include <string>

#ifndef __GRAPH_H__
#define __GRAPH_H__

//typedef std::string Node;
//typedef std::pair<std::string, std::string> Key;
typedef unsigned int Node;
typedef std::pair<unsigned int, unsigned int> Key;

class Edge
{
public:
	Node left;
	Node right;
	
	double weight;
	
	Edge(Node a, Node b, double weight=1.0);
};

class Graph
{
protected:
	bool loadFromPairs(std::string filename);
	bool loadFromWeights(std::string filename);
	bool valid;
	
public:
	std::set<Node>nodes;
	std::map<Key, Edge *>edges;

	Graph(const std::string filename);
    Graph();
	bool isValid();
    double linksBetween(std::set<Node> a, std::set<Node> b);
    void setWeight(Node a, Node b, double weight);
    
    void addNode(Node node);
    void addEdge(Node a, Node b, double weight=1.0);
};

#endif
