#define DESCRIPTION "Read a target corpus and incrementally build a semantic network and a dendrogram."
#define USAGE       "hierize path/to/corpus.target_corpus output/dir saveInterval samplesPerUpdate"
#define NUM_ARGS    4

#include "corpus.h"
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

Corpus *targetCorpus;
Graph *graph;
Distance *similarity;
progressbar *corpusProgress;
unsigned int documentIndex = 0;
int saveInterval;
int samplesPerUpdate;
Dendrogram *dendrogram;

string filenameHandle;
string outputDirectory;

void eachDocument(Word target, Document document, bool isNewTarget);

string makeFilenameHandle(const char *filename)
{
    char buffer[120] = "\0";
    strcpy(buffer,filename);
    
    char *handle = buffer;
    if (strrchr(buffer,'/') != NULL) {
        handle = strrchr(buffer,'/')+1;
    }
    
    return string(handle);
}

void usage()
{
    cout << DESCRIPTION << endl;
    cout << endl;
    cout << "Usage: " << USAGE << endl;
    
    exit(1);
}

void save(bool isFinal=false);

int main(int argc, const char **argv)
{
    if (argc != NUM_ARGS+1) {
        usage();
    }
    
    srand(time(0));
    
    filenameHandle = makeFilenameHandle(argv[1]);
    outputDirectory = string(argv[2]);
    saveInterval = atoi(argv[3]);
    samplesPerUpdate = atoi(argv[4]);
    
    // Process each document (sentence) from the target corpus
    targetCorpus = new Corpus(string(argv[1]));
    graph = new Graph();
    similarity = new Distance();
    dendrogram = new Dendrogram(graph);
    corpusProgress = progressbar_new("Processing",targetCorpus->size());
    targetCorpus->eachDocument(&eachDocument);
    save(true);
    progressbar_finish(corpusProgress);
}

void save(bool isFinal)
{
    char documentIndexString[10];
    sprintf(documentIndexString,"%d",documentIndex);
    
    // Save graph
    ostringstream oss;
    oss << outputDirectory << "/" << filenameHandle << "." << (isFinal ? "final" : documentIndexString) << ".graph";
    ofstream fout(oss.str().c_str());
    fout << graph->toString() << endl;
    fout.close();
    
    // Save wordmap
    ostringstream wordmapFilename;
    wordmapFilename << outputDirectory << "/" << filenameHandle << "." << (isFinal ? "final" : documentIndexString) << ".wordmap";
    ofstream wordmapOut(wordmapFilename.str().c_str());
    wordmapOut << targetCorpus->wordmapToString() << endl;
    wordmapOut.close();
    
    // Save dendrogram
    ostringstream dendrogramFilename;
    dendrogramFilename << outputDirectory << "/" << filenameHandle << "." << (isFinal ? "final" : documentIndexString) << ".dendrogram";
    ofstream dendrogramOut(dendrogramFilename.str().c_str());
    dendrogramOut << dendrogram->toString() << endl;
    dendrogramOut.close();
    
    // Save dot
    ostringstream dotFilename;
    dotFilename << outputDirectory << "/" << filenameHandle << "." << (isFinal ? "final" : documentIndexString) << ".dot";
    ofstream dotOut(dotFilename.str().c_str());
    dotOut << dendrogram->toDot(targetCorpus) << endl;
    dotOut.close();
}

void eachDocument(Word target, Document document, bool isNewTarget)
{
    similarity->updateGraph(target,document,graph);
    
    if (isNewTarget) {
        dendrogram->addLeaf(target);
    }
    
    for(int i=0;i<samplesPerUpdate;i++) {
	    dendrogram->sample();
    }
    
    documentIndex++;
    
    if (documentIndex % saveInterval == 0) {
    	save();
    }
    
    progressbar_inc(corpusProgress);
}
