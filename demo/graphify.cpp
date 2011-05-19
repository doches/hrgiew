
#define DESCRIPTION "Read a target corpus and incrementally build a semantic network of target words."
#define USAGE       "graphify path/to/corpus.target_corpus"
#define NUM_ARGS    1

#include "corpus.h"
#include "graph.h"
#include "distance.h"
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

Corpus *targetCorpus;
Graph *graph;
Distance *distance;

string filenameHandle;

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
    
    cout << filenameHandle << endl;
}