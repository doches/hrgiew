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
        test_equal(34,graph->nodes.size(), "incorrect number of nodes in karate.pairs");	
        delete graph;
    
    context("triangle.weights");
        graph = new Graph("data/triangle.weights");
        testcase(graph->isValid(), "failed to load valid graph from data/triangle.weights");
        test_equal(12,graph->edges.size(), "incorrect number of edges in triangle.weights");
        test_equal(6,graph->nodes.size(), "incorrect number of nodes in triangle.weights");
        std::set<Node> nodes_ab, nodes_z, nodes_c;
        nodes_c.insert(3);
        nodes_ab.insert(1);
        test_equal(1.0,graph->linksBetween(nodes_ab,nodes_c), "Incorrect link weight between 1,3");
        graph->addEdge(1,3,0.5);
        test_equal(0.5,graph->linksBetween(nodes_ab,nodes_c), "Failed to set link weight between 1,3");
        graph->addNode(99);
        graph->addEdge(99,1,0.9);
        nodes_ab.insert(2);
        nodes_z.insert(99);
        test_equal(0.9,graph->linksBetween(nodes_ab,nodes_z), "Incorrect link weight between [1,2] , [99]");
        test_equal(0.0,graph->linksBetween(nodes_c,nodes_z), "Incorrect link weight between [99] , [3]");
        delete graph;
	
	return this->didPass();
}
