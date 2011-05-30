#include "dendrogram.h"
#include "graph.h"
#include "progressbar.h"
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <fstream>

int main(int argc, const char **argv)
{
    Graph *graph = new Graph(argv[1]);
    Dendrogram *dendro = new Dendrogram(graph);
    Dendrogram *bestDendrogram = NULL;
    double bestLikelihood = 1.0f;
    
    int iterations = atoi(argv[2]);
    
    int printEveryN = 100;
    
    srand(time(0));
    
    progressbar *progress = progressbar_new("Sampling",iterations);
    for (int i=0; i<iterations; i++) {
        dendro->sample();
        double logLikelihood = dendro->likelihood();
        if (i % printEveryN == 0) {
            printf("Iteration %d = %f\n",i,logLikelihood);
        }
        progressbar_inc(progress);
        
        if (bestDendrogram == NULL || logLikelihood > bestLikelihood) {
            bestLikelihood = logLikelihood;
            if (bestDendrogram != NULL) {
                delete bestDendrogram;
            }
            bestDendrogram = new Dendrogram(dendro);
            char filename[80];
            sprintf(filename,"fig4.%d.%f.dot",i,bestLikelihood);
            std::ofstream fout(filename);
            fout << bestDendrogram->toDot() << std::endl;
            fout.close();
//            printf("New best: %f\n",bestLikelihood);
        }
    }
    progressbar_finish(progress);
    bestDendrogram->print();
}
