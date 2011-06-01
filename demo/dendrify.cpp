#define DESCRIPTION "Read a saved graph and output a naive dendrogram"
#define USAGE       "dendrify file.graph"
#define NUM_ARGS    1

#include "graph.h"
#include "distance.h"
#include "dendrogram.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

void usage()
{
    cout << DESCRIPTION << endl;
    cout << endl;
    cout << "Usage: " << USAGE << endl;
    
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != NUM_ARGS+1) {
        usage();
    }
    
    srand(time(0));
    
    Graph *graph = new Graph(argv[1]);
    Dendrogram *dendrogram = new Dendrogram(graph);
    
    cout << dendrogram->toString() << endl;
}