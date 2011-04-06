#include "graph.h"
#include "test.h"

class GraphTest : public Test
{
public:
	bool run();
	GraphTest();
};

GraphTest::GraphTest()
{
	this->name = "Graph";
}

bool GraphTest::run()
{
    context("triangle.pairs");
        Graph *graph = new Graph("data/triangle.pairs");
        testcase(graph->isValid(), "failed to load from data/triangle.pairs");
        testcase(graph->edges.size() == 12, "incorrect number of edges in triangle.pairs");
        testcase(graph->nodes.size() == 6, "incorrect number of nodes in triangle.pairs");
        delete graph;
	
    context("karate.pairs");
        graph = new Graph("data/karate.pairs");
        testcase(graph->isValid(), "failed to load from data/karate.pairs");
        testcase(graph->nodes.size() == 34, "incorrect number of nodes in karate.pairs");	
        delete graph;
    
    context("triangle.weights");
        graph = new Graph("data/triangle.weights");
        testcase(graph->isValid(), "failed to load valid graph from data/triangle.weights");
        testcase(graph->edges.size() == 12, "incorrect number of edges in triangle.weights");
        testcase(graph->nodes.size() == 6, "incorrect number of nodes in triangle.weights");
        std::set<Node> nodes_ab, nodes_z, nodes_c;
        nodes_c.insert("C");
        nodes_ab.insert("A");
        testcase(graph->linksBetween(nodes_ab,nodes_c) == 1.0, "Incorrect link weight between A,C");
        graph->setWeight("A","C",0.5);
        testcase(graph->linksBetween(nodes_ab,nodes_c) == 0.5, "Failed to set link weight between A,C");
        graph->addNode("Z");
        graph->addEdge("Z","A",0.9);
        nodes_ab.insert("B");
        nodes_z.insert("Z");
        testcase(graph->linksBetween(nodes_ab,nodes_z) == 0.9, "Incorrect link weight between [A,B] , [Z]");
        testcase(graph->linksBetween(nodes_c,nodes_z) == 0.0, "Incorrect link weight between [Z] , [C]");
        delete graph;
	
	return this->didPass();
}
