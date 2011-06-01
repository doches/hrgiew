#define DESCRIPTION "Read a saved graph and dendrogram (in pointer format) and resample forever"
#define USAGE       "resample file.graph file.dendrogram output/dir"
#define NUM_ARGS    2

#include "graph.h"
#include "distance.h"
#include "progressbar.h"
#include "dendrogram.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>

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
    Dendrogram *dendrogram = new Dendrogram(graph,argv[2]);
}