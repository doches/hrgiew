#include "graph.h"
#include <math.h>
#include "logger.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <cstring>

Edge::Edge(Node a, Node b, double weight)
{
	this->left = a;
	this->right = b;
	this->weight = weight;
}

std::string Edge::toString()
{
    std::ostringstream str;
    str << this->left << " <- " << this->weight << " -> " << this->right;
    return str.str();
}

Graph::Graph(std::string filename)
{
	this->valid = false;
	if (filename.rfind(".pairs")!=std::string::npos) {
		this->valid = loadFromPairs(filename);
	} else if (filename.rfind(".weights")!=std::string::npos || filename.rfind(".graph")!=std::string::npos) {
		this->valid = loadFromWeights(filename);
    }
    this->threshold = 0.0f;
}

Graph::Graph()
{
    this->valid = true;
    this->threshold = 0.0f;
}

std::string Graph::toString()
{
    const double epsilon = 0.00001f;
    std::ostringstream str;
    for (std::map<Key, Edge *>::iterator iterator=edges.begin(); iterator!=edges.end(); iterator++) {
        if (iterator->second != NULL) { // WTF, Holmes...
            if (fabs(iterator->second->weight-epsilon) > epsilon) {
                str << iterator->first.first << "\t" << iterator->first.second << "\t" << iterator->second->weight << std::endl;
            } 
        }
    }
    
    return str.str();
}

bool Graph::loadFromWeights(const std::string filename)
{
    FILE *fin = fopen(filename.c_str(),"r");
    char line[80];
    if (fin) {
        while (!feof(fin)) {
            char *res = fgets(line,80,fin);
            
            if (res && strchr(line,'\t')!=NULL) {
                Node left = Node(atoi(strtok(line,"\t")));
                Node right = Node(atoi(strtok(NULL,"\t")));
                double weight = atof(strtok(NULL,"\t\n"));
                
                addEdge(left,right,weight);
            }
        }
        fclose(fin);
    } else {
        return false;
    }
    
    return true;
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
			Node left = Node(atoi(line.substr(0,tabPosition).c_str()));
			Node right = Node(atoi(line.substr(tabPosition+1,std::string::npos).c_str()));
            
            addNode(left);
            addNode(right);
            addEdge(left,right);
		}
	}
	fin.close();
	
	return true;
}

double Graph::linksBetween(std::set<Node> a, std::set<Node> b)
{
    CacheKey key = CacheKey(a,b);
    
    std::map<CacheKey,double>::iterator cache = linksCache.find(key);
    if (cache != linksCache.end()) {
        return cache->second;
    }

    double linkCount = 0;

    for (std::set<Node>::iterator aIter=a.begin(); aIter != a.end(); aIter++) {
        for (std::set<Node>::iterator bIter=b.begin(); bIter != b.end(); bIter++) {
            Edge *candidate = edges[Key(*aIter,*bIter)];
            if (candidate != NULL && candidate->weight >= threshold) {
                linkCount += 1;
                //linkCount += candidate->weight;
            }
        }
    }
    
    linksCache[key] = linkCount;
    
    return linkCount;
}

bool Graph::isValid()
{
	return this->valid;
}

void Graph::addNode(Node node)
{
    this->nodes.insert(node);
}

void Graph::addEdge(Node left, Node right, double weight)
{
    this->nodes.insert(left);
    this->nodes.insert(right);
    
    Key lr = Key(left,right);
    Key rl = Key(right,left);
    if (this->edges.find(lr) != this->edges.end() && this->edges[lr] != NULL) {
        this->edges[lr]->weight = weight;
    } else {
        Edge *lrEdge = new Edge(left,right,weight);
        this->edges.insert(std::pair<Key,Edge *>(lr,lrEdge));
    }
    
    if (this->edges.find(rl) != this->edges.end() && this->edges[rl] != NULL) {
        this->edges[rl]->weight = weight;
    } else {    
        Edge *rlEdge = new Edge(right,left,weight);
        this->edges.insert(std::pair<Key,Edge *>(rl,rlEdge));
    }
}

void Graph::setWeight(Node left, Node right, double weight)
{
    this->addEdge(left,right,weight);
}
