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
//    
//    for (int i=0; i<100; i++) {
//        dendro->sample();
//    }
//    printf("dendro 1: %f",dendro->likelihood());
//    dendro->print();
//    Dendrogram *hold = new Dendrogram(dendro);
//    printf("hold 1: %f",dendro->likelihood());
//    hold->print();
//    for (int i=0; i<100; i++) {
//        dendro->sample();
//    }
//    printf("dendro 2: %f",dendro->likelihood());
//    dendro->print();
//    printf("hold 2: %f",dendro->likelihood());
//    hold->print();
    
    
	delete dendro;
	delete graph;
	
	return this->didPass();
}
