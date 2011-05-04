//
//  learner.cpp
//  hrgiew
//
//  Created by Trevor Fountain on 4/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "corpus.h"
#include "dendrogram.h"
#include "graph.h"
#include "consensus.h"
#include "progressbar.h"
#include "statusbar.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <time.h>
using namespace std;

typedef std::map<Word, unsigned int> WordVector;
typedef std::map<Word, WordVector> WordVectorMap;
typedef std::map<Word, double> MagnitudeMap;

void eachDocument(Word target, Document doc, bool newTarget);
unsigned int dot(WordVector a, WordVector b);

WordVectorMap wordCounts;
Graph *graph;
Dendrogram *dendrogram;
MagnitudeMap magnitude;
progressbar *status;
unsigned int documentIndex = 0;

int samplesPerUpdate = 0;
int postCorpusSamples = 0;
int consensusSpread = 0;
int consensusSamples = 0;

std::string *handle;

void usage()
{
    cout << "Reads a target corpus and produces a consensus hierarchy" << endl;
    cout << endl;
    cout << "Usage: ./learner <path/to/corpus> <samplesPerUpdate> <postCorpusSamples> <consensusSpread> <consensusSamples>" << endl;
    exit(1);
}

void makeHandle()
{
    char filename[80] = "\0";
    strcpy(filename,handle->c_str());
    
    char *fname = strrchr(filename,'/')+1;
    
    ostringstream oss;
    oss << fname << "-" << samplesPerUpdate << "-" << postCorpusSamples << "-" << consensusSpread << "-" << consensusSamples;
    
    handle = new std::string(oss.str());
    cout << "Saving trial into " <<*handle << ".*" << endl;
}

int main(int argc, char **argv)
{
    if (argc != 6) {
        usage();
    }
    
    srand(time(0));
    
    handle = new std::string(argv[1]);
    Corpus *targetCorpus = new Corpus(handle->c_str());
    samplesPerUpdate = atoi(argv[2]);
    postCorpusSamples = atoi(argv[3]);
    consensusSpread = atoi(argv[4]);
    consensusSamples = atoi(argv[5]);
    
    makeHandle();
    
    graph = new Graph();
    dendrogram = new Dendrogram(graph);
    status = progressbar_new("Processing",targetCorpus->size());
    targetCorpus->eachDocument(&eachDocument);
    progressbar_finish(status);
    
    cout << dendrogram->likelihood() << endl;
    
    Dendrogram *bestDendrogram = NULL;
    double bestScore = 0.0;
    
    progressbar *sampleProgress = progressbar_new("Post-sampling",postCorpusSamples);
    for (int i=-1; i<postCorpusSamples; i++) {
        dendrogram->sample();
        
        if (dendrogram->likelihood() > bestScore || bestDendrogram == NULL) {
            if (bestDendrogram != NULL) {
                delete bestDendrogram;
            }
            bestDendrogram = new Dendrogram(dendrogram);
            bestScore = dendrogram->likelihood();
        }
        progressbar_inc(sampleProgress);
    }
    progressbar_finish(sampleProgress);
    
    cout << endl << "*** Final sampled dendrogram ***" << endl;
    cout << "sampled likelihood: " << bestDendrogram->likelihood() << std::endl;
    bestDendrogram->print(targetCorpus);
    cout << endl;
    
    int sampleCount = consensusSpread;
    DendrogramSet samples;
    progressbar *consensusProgress = progressbar_new("Seeking consensus",sampleCount);
    for (int i=0; i<sampleCount; i++) {
        bestDendrogram->sample();
        if (i % (sampleCount / consensusSamples) == 0) {
            Dendrogram *sampledDendrogram = new Dendrogram(bestDendrogram);
            samples.insert(sampledDendrogram);
            cout << endl << "*** Consensus dendrogram ***"<<endl;
            cout << "sampled likelihood: " << sampledDendrogram->likelihood() << std::endl;
            cout << endl;
        }
        progressbar_inc(consensusProgress);
    }
    progressbar_finish(consensusProgress);
    
    Consensus *hierarchy = new Consensus(samples, graph, targetCorpus);
    cout << endl <<"*** Consensus Hierarchy ***" << endl;
    std::cout << hierarchy->toString(targetCorpus) << std::endl;
    
    char temporaryFilename[120];
    sprintf(temporaryFilename,"%s.dot",handle->c_str());
    std::ofstream fout(temporaryFilename);
    fout << hierarchy->toDot(targetCorpus) << std::endl;
    fout.close();
    
    sprintf(temporaryFilename,"%s.matrix",handle->c_str());
    std::ofstream matrix(temporaryFilename);
    matrix << hierarchy->toMatrix(targetCorpus) << std::endl;
    matrix.close();
}

void printContexts()
{
    // Print out contexts
    for (WordVectorMap::iterator mapIterator = wordCounts.begin(); mapIterator!=wordCounts.end(); mapIterator++) {
        Word target = (*mapIterator).first;
        cout << target << ": ";
        for (WordVector::iterator countIterator = mapIterator->second.begin(); countIterator!=mapIterator->second.end(); countIterator++) {
            Word context = countIterator->first;
            unsigned int count = countIterator->second;
            
            cout << context << "("<<count<<") ";
        }
        cout << endl;
    }
}

void eachDocument(Word target, Document doc, bool newTarget)
{
    // Update contexts
    for (Document::iterator iter = doc.begin(); iter != doc.end(); iter++) {
        wordCounts[target][*iter]++;
    }
    WordVector targetVector = wordCounts[target];
    
    // Update magnitude for target word
    double sum = 0.0;
    for (WordVector::iterator wordIterator=targetVector.begin(); wordIterator!=targetVector.end(); wordIterator++) {
        sum += pow(wordIterator->second,2);
    }
    double targetMagnitude = sqrt(sum);
    magnitude[target] = targetMagnitude;
    
    // Update graph
    for (WordVectorMap::iterator mapIterator = wordCounts.begin(); mapIterator!=wordCounts.end(); mapIterator++) {
        Word otherTarget = mapIterator->first;
        
        if (otherTarget != target) {
            WordVector otherVector = mapIterator->second;
            double otherMagnitude = magnitude[otherTarget];
            
            double cosine = dot(targetVector,otherVector) / (targetMagnitude * otherMagnitude);
            
            if (cosine > 0) {
                graph->addEdge(target,otherTarget,cosine);
            }
        }
    }
    
    if (newTarget) {
        dendrogram->addLeaf(target);
    }
    for (int i=0; i<samplesPerUpdate; i++) {
        dendrogram->sample();
    }
    
    documentIndex++;
    
    if (documentIndex > 0 && documentIndex % 10 == 0) {
//        char fname[20];
//        sprintf(fname,"graph.%d.weights",documentIndex);
//        std::ofstream fout(fname);
//        fout << graph->toString();
//        fout.close();
    }
    
    progressbar_inc(status);
}

unsigned int dot(WordVector a, WordVector b)
{
    if (a.size() > b.size()) {
        return dot(b,a);
    }
    
    unsigned int sum = 0;
    for (WordVector::iterator wordIterator=a.begin(); wordIterator!=a.end(); wordIterator++) {
        sum += wordIterator->second * b[wordIterator->first];
    }
    
    return sum;
}