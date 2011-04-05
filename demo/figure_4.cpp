#include "dendrogram.h"
#include "graph.h"
#include "progressbar.h"
#include <cmath>

int main(int argc, const char **argv)
{
    Graph *graph = new Graph(argv[1]);
    Dendrogram *dendro = new Dendrogram(graph);
    
    int iterations = atoi(argv[2]);
    
    progressbar *progress = progressbar_new("Sampling",iterations);
    for (int i=0; i<iterations; i++) {
        dendro->sample();
        if (i%100 == 0) {
            dendro->print();
            printf("Iteration %d = %f\n",i,log10(dendro->likelihood()));
        }
        progressbar_inc(progress);
    }
    progressbar_finish(progress);
}