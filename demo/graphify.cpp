
#define DESCRIPTION "Read a target corpus and incrementally build a semantic network of target words."
#define USAGE       "graphify path/to/corpus.target_corpus soutput/dir"
#define NUM_ARGS    2

#define SAVE_INTERVAL 250

#include "corpus.h"
#include "graph.h"
#include "distance.h"
#include "progressbar.h"
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

int main(int argc, const char **argv)
{
    if (argc != NUM_ARGS+1) {
        usage();
    }
    
    filenameHandle = makeFilenameHandle(argv[1]);
    outputDirectory = string(argv[2]);
    
    // Process each document (sentence) from the target corpus
    targetCorpus = new Corpus(string(argv[1]));
    graph = new Graph();
    similarity = new Distance();
    corpusProgress = progressbar_new("Processing",targetCorpus->size());
    targetCorpus->eachDocument(&eachDocument);
    progressbar_finish(corpusProgress);
}

void eachDocument(Word target, Document document, bool isNewTarget)
{
    similarity->updateGraph(target,document,graph);
    
    documentIndex++;
    
    if (documentIndex % SAVE_INTERVAL == 0 && documentIndex) {
        // Save graph
        ostringstream oss;
        oss << outputDirectory << "/" << filenameHandle << "." << documentIndex << ".graph";
        ofstream fout(oss.str().c_str());
        fout << graph->toString() << endl;
        fout.close();
        
        // Save wordmap
        ostringstream wordmapFilename;
        wordmapFilename << outputDirectory << "/" << filenameHandle << "." << documentIndex << ".wordmap";
        ofstream wordmapOut(wordmapFilename.str().c_str());
        wordmapOut << targetCorpus->wordmapToString() << endl;
        wordmapOut.close();
    }
    
    progressbar_inc(corpusProgress);
}