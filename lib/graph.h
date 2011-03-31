#include <set>
#include <map>
#include <string>

#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef std::string Node;
typedef std::pair<std::string, std::string> Key;

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
	bool valid;
	
public:
	std::set<Node>nodes;
	std::map<Key, Edge *>edges;
	std::map<Node, unsigned int>degrees;

	Graph(const std::string filename);
	bool isValid();
	unsigned int degree(Node node);
};

#endif
