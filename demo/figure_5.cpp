#include "dendrogram.h"
#include "graph.h"
#include "progressbar.h"
#include "consensus.h"
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <iostream>

int main(int argc, const char **argv)
{
    Graph *graph = new Graph(argv[1]);
    Dendrogram *dendro = new Dendrogram(graph);
    Dendrogram *bestDendrogram = NULL;
    double bestLikelihood = 1.0f;
    
    int iterations = atoi(argv[2]);
    
    srand(time(0));
    
    progressbar *progress = progressbar_new("Sampling",iterations);
    for (int i=0; i<iterations; i++) {
        dendro->sample();
        double logLikelihood = dendro->likelihood();
        progressbar_inc(progress);
        
        if (bestDendrogram == NULL || logLikelihood > bestLikelihood) {
            bestLikelihood = logLikelihood;
            if (bestDendrogram != NULL) {
                delete bestDendrogram;
            }
            bestDendrogram = new Dendrogram(dendro);
        }
    }
    progressbar_finish(progress);
    
    DendrogramSet samples;
    for (int i=0; i<500; i++) {
        bestDendrogram->sample();
        if (i % 100 == 0) {
            samples.insert(new Dendrogram(bestDendrogram));
        }
    }
    
    Consensus *hierarchy = new Consensus(samples, graph);
    std::cout << hierarchy->toDot() << std::endl;
}