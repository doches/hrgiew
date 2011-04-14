#include "graph.h"
#include "test.h"
#include "dendrogram.h"

class DendrogramTest : public Test
{
public:
	bool run();
	DendrogramTest();
};

DendrogramTest::DendrogramTest()
{
	this->name = "Dendrogram";
}

bool DendrogramTest::run()
{
    context("triangle.pairs");
	Graph *graph = new Graph("data/triangle.pairs");
    Dendrogram *dendro = new Dendrogram(graph);
    testcase(dendro != NULL, "Initialized Dendrogram is NULL!");
    testcase(dendro->getRoot() != NULL, "Dendrogram has NULL root");
    testcase(((InternalNode *)(dendro->getRoot()))->getLeft() != NULL, "Dendrogram root has NULL left child");
    testcase(((InternalNode *)(dendro->getRoot()))->getRight() != NULL, "Dendrogram root has NULL right child");
    
	delete dendro;
	delete graph;
    
    context("triangle.weights");
	graph = new Graph("data/triangle.weights");
    testcase(graph->isValid(), "failed to load valid graph from data/triangle.weights");
    dendro = new Dendrogram(graph);
    testcase(dendro != NULL, "Initialized Dendrogram is NULL!");
    testcase(dendro->getRoot() != NULL, "Dendrogram has NULL root");
    for (int i=0; i<100; i++) {
        dendro->sample();
    }
    testcase(dendro->likelihood() < 0.0f, "Invalid likelihood for post-sampled dendrogram");
    graph->addNode(99);
    graph->addEdge(99,1,0.9);
    std::set<Node> nodes_ab, nodes_z;
    nodes_ab.insert(1);
    nodes_ab.insert(2);
    nodes_z.insert(99);
    testcase(graph->linksBetween(nodes_ab,nodes_z) == 0.9, "Incorrect link weight between [A,B] , [Z]");
    dendro->addLeaf(99,1);
    testcase(graph->linksBetween(nodes_ab,nodes_z) == 0.9, "Incorrect link weight between [A,B] , [Z]");
	
	return this->didPass();
}
