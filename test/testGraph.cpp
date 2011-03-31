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
	Graph *graph = new Graph("data/triangle.pairs");
	
	testcase(graph->isValid(), "failed to load from data/triangle.pairs");
	testcase(graph->edges.size() == 6, "incorrect number of edges in triangle.pairs");
	testcase(graph->nodes.size() == 6, "incorrect number of nodes in triangle.pairs");
	testcase(graph->degree("A") == 2, "wrong degree for 'A'");
	testcase(graph->degree("D") == 3, "wrong degree for 'D'");
	
	delete graph;
	
	graph = new Graph("data/karate.pairs");
	testcase(graph->isValid(), "failed to load from data/karate.pairs");
	testcase(graph->edges.size() == 155, "incorrect number of edges in karate.pairs");
	testcase(graph->nodes.size() == 34, "incorrect number of nodes in karate.pairs");
	
	delete graph;
	
	return this->didPass();
}
