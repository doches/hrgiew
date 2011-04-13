#include "graph.h"
#include "logger.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

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
	} else if (filename.rfind(".weights")!=std::string::npos) {
		this->valid = loadFromWeights(filename);
    }
}

Graph::Graph()
{
    this->valid = true;
}

bool Graph::loadFromWeights(const std::string filename)
{
//    FILE *fin = fopen(filename.c_str(),"r");
//    char line[80];
//    if (fin) {
//        while (!feof(fin)) {
//            fgets(line,80,fin);
//            
//            if (strchr(line,'\t')!=NULL) {
//                std::string left = std::string(strtok(line,"\t"));
//                std::string right = std::string(strtok(NULL,"\t"));
//                double weight = atof(strtok(NULL,"\t\n"));
//                
//                addNode(left);
//                addNode(right);
//                addEdge(left,right,weight);
//            }
//        }
//        fclose(fin);
//    } else {
//        return false;
//    }
    
    return true;
}

bool Graph::loadFromPairs(const std::string filename)
{
	std::ifstream fin(filename.c_str(), std::ifstream::in);
	
	if (!fin.is_open()) {
		return false;
	}
	
//	std::string line;
//	while (fin.good())
//	{
//		std::getline(fin, line);
//		size_t tabPosition = line.find("\t");
//		if (tabPosition != std::string::npos) {
//			std::string left = line.substr(0,tabPosition);
//			std::string right = line.substr(tabPosition+1,std::string::npos);
//            
//            addNode(left);
//            addNode(right);
//            addEdge(left,right);
//		}
//	}
//	fin.close();
	
	return true;
}

double Graph::linksBetween(std::set<Node> a, std::set<Node> b)
{
    double linkCount = 0;

    for (std::set<Node>::iterator aIter=a.begin(); aIter != a.end(); aIter++) {
        for (std::set<Node>::iterator bIter=b.begin(); bIter != b.end(); bIter++) {
            Edge *candidate = edges[Key(*aIter,*bIter)];
            if (candidate != NULL) {
                linkCount += candidate->weight;
            }
        }
    }
    
    return linkCount;
}

bool Graph::isValid()
{
	return this->valid;
}

void Graph::addNode(Node node)
{
    Node newNode = Node(node);
    this->nodes.insert(newNode);
}

void Graph::addEdge(Node left, Node right, double weight)
{
    this->nodes.insert(left);
    this->nodes.insert(right);
    
    Edge *edge = new Edge(left,right,weight);
    this->edges.insert(std::pair<Key,Edge *>(Key(left,right), edge));
    this->edges.insert(std::pair<Key,Edge *>(Key(right,left), edge));
}

void Graph::setWeight(Node left, Node right, double weight)
{
    Edge *edge = this->edges[Key(left,right)];
    if (edge == NULL) {
        edge = this->edges[Key(right,left)];
    }
    
    edge->weight = weight;
}