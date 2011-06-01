#define DESCRIPTION "Read a saved graph and dendrogram (in pointer format), resample, and infer a consensus hierarchy"
#define USAGE       "consensus file.graph file.dendrogram sampleSpread sampleCount"
#define NUM_ARGS    4

#include "graph.h"
#include "distance.h"
#include "progressbar.h"
#include "dendrogram.h"
#include "consensus.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <list>

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
    int sampleSpread = atoi(argv[3]);
    int sampleCount = atoi(argv[4]);
    int sampleEvery = sampleSpread / sampleCount;
    
    DendrogramSet samples;

    for (int sampleIndex = 0; sampleIndex < sampleSpread; sampleIndex++) {
        dendrogram->sample();
        
        if (sampleIndex % sampleEvery == 0) {
            cout << "Sampling at " << sampleIndex << " / " << sampleSpread << endl;
            
            samples.insert(new Dendrogram(dendrogram));
        }
    }
    
    Consensus *consensus = new Consensus(samples,graph);
    
    cout << consensus->toString() << endl;
}