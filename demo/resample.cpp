#define DESCRIPTION "Read a saved graph and dendrogram (in pointer format) and resample forever"
#define USAGE       "resample file.graph file.dendrogram output/dir prefix printEvery [sampleCount]"
#define NUM_ARGS    5

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
    if (argc < NUM_ARGS+1) {
        usage();
    }
    
    srand(time(0));
    
    Graph *graph = new Graph(argv[1]);
    Dendrogram *dendrogram = new Dendrogram(graph,argv[2]);
    string outputDir = string(argv[3]);
    string prefix = string(argv[4]);
    int printEvery = atoi(argv[5]);
    int sampleCount = -1;
    if (argc == 7) {
        sampleCount = atoi(argv[6]);
    }
    
    unsigned int sampleIndex = 0;
    Dendrogram *best = NULL;
    double bestLikelihood = 0.0f;
    unsigned int bestIndex = 0;
    bool saved = false;
    char logFilename[80];
    sprintf(logFilename,"%s/resample.log",outputDir.c_str());
    ofstream logFile(logFilename);
    while(true)
    {
        dendrogram->sample();
        sampleIndex++;
        
        if (dendrogram->likelihood() > bestLikelihood || best == NULL) {
            best = new Dendrogram(dendrogram);
            bestLikelihood = dendrogram->likelihood();
            saved = false;
            bestIndex = sampleIndex;
        }
        
        if (sampleIndex % printEvery == 0) {
            printf("%u\t%f\n",sampleIndex,dendrogram->likelihood());
            logFile << sampleIndex << "\t" << dendrogram->likelihood() << endl;
            logFile.flush();
            
            if (!saved) {
                std::string toStr = best->toString();
                
                char filename[80];
                sprintf(filename,"%s/%s.%u.%f.dendrogram",outputDir.c_str(),prefix.c_str(),bestIndex,bestLikelihood);
                ofstream fout(filename);
                fout << toStr;
                fout.close();
                
                sprintf(filename,"%s/%s.dendrogram",outputDir.c_str(),prefix.c_str());
                ofstream fout_best(filename);
                fout_best << toStr;
                fout_best.close();
                
                saved = true;
            }
        }
        
        // Terminate if A) we were given a sample count and B) we've exceeded it
        if (sampleCount != -1 && (int)sampleIndex >= sampleCount) {
            break;
        }
    }
    logFile.close();
}
