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
	Graph *graph = new Graph("data/triangle.pairs");
    Dendrogram *dendro = new Dendrogram(graph);
    testcase(dendro != NULL, "Initialized Dendrogram is NULL!");
    testcase(dendro->getRoot() != NULL, "Dendrogram has NULL root");
    testcase(((InternalNode *)(dendro->getRoot()))->getLeft() != NULL, "Dendrogram root has NULL left child");
    testcase(((InternalNode *)(dendro->getRoot()))->getRight() != NULL, "Dendrogram root has NULL right child");
    
    printf("%f",dendro->likelihood());
    dendro->print();
    
    for (int i=0; i<100; i++) {
        printf("%f\n",dendro->sample());
    }
    printf("%f",dendro->likelihood());
    dendro->print();
    
	delete dendro;
	delete graph;
	
	return this->didPass();
}
