#include "consensus.h"
#include "test.h"
#include "dendrogram.h"
#include "graph.h"

class ConsensusTest : public Test
{
public:
	bool run();
	ConsensusTest();
};

ConsensusTest::ConsensusTest()
{
	this->name = "Consensus";
}

bool ConsensusTest::run()
{
    context("triangle.weights");
    Graph *graph = new Graph("data/triangle.weights");
    testcase(graph->isValid(), "failed to load graph");
    Dendrogram *dendro = new Dendrogram(graph);
    for (int i=0; i<200; i++) {
        dendro->sample();
    }
    DendrogramSet best;
    for (int i=0; i<50; i++) {
        dendro->sample();
        if (i%10 == 0) {
            best.insert(new Dendrogram(dendro));
        }
    }
    Consensus *hierarchy = new Consensus(best, graph);
    testcase(hierarchy != NULL, "failed to allocate/initialize Consensus");
    std::cout << hierarchy->toString() << std::endl;
    delete hierarchy;
    delete graph;
    delete dendro;
	
	return this->didPass();
}
