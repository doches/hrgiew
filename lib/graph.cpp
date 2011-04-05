#include "graph.h"
#include "logger.h"
#include <fstream>
#include <iostream>

Edge::Edge(Node a, Node b, double weight)
{
	this->left = a;
	this->right = b;
	this->weight = weight;
}

Graph::Graph(std::string filename)
{
	this->valid = false;
	if (filename.rfind(".pairs")!=std::string::npos) {
		this->valid = loadFromPairs(filename);
	}
}

bool Graph::loadFromPairs(const std::string filename)
{
	std::ifstream fin(filename.c_str(), std::ifstream::in);
	
	if (!fin.is_open()) {
		return false;
	}
	
	std::string line;
	while (fin.good())
	{
		std::getline(fin, line);
		size_t tabPosition = line.find("\t");
		if (tabPosition != std::string::npos) {
			std::string left = line.substr(0,tabPosition);
			std::string right = line.substr(tabPosition+1,std::string::npos);
			
			Node leftNode = Node(left);
			Node rightNode = Node(right);
			
			this->nodes.insert(leftNode);
			this->nodes.insert(rightNode);
			
			this->degrees[leftNode]++;
			this->degrees[rightNode]++;
			
			Edge *edge = new Edge(leftNode, rightNode);
			this->edges.insert(std::pair<Key,Edge *>(Key(left,right), edge));
			this->edges.insert(std::pair<Key,Edge *>(Key(right,left), edge));
		}
	}
	fin.close();
	
	return true;
}

unsigned int Graph::linksBetween(std::set<Node> a, std::set<Node> b)
{
    unsigned int linkCount = 0;

    for (std::set<Node>::iterator aIter=a.begin(); aIter != a.end(); aIter++) {
        for (std::set<Node>::iterator bIter=b.begin(); bIter != b.end(); bIter++) {
            Edge *candidate = edges[Key(*aIter,*bIter)];
            if (candidate != NULL) {
                linkCount++;
            }
        }
    }
    
    return linkCount;
}

unsigned int Graph::degree(Node node)
{
	return this->degrees[node];
}

bool Graph::isValid()
{
	return this->valid;
}
