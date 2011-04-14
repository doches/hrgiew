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

#include <math.h>
#include <iostream>
#include <fstream>
#include <map>
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
unsigned int documentIndex = 0;

int main(int argc, char **argv)
{
    Corpus *targetCorpus = new Corpus(argv[1]);
    graph = new Graph();
    dendrogram = new Dendrogram(graph);
    targetCorpus->eachDocument(&eachDocument);
    
    cout << dendrogram->likelihood() << endl;
    dendrogram->print();
    
//    // Print out contexts
//    for (WordVectorMap::iterator mapIterator = wordCounts.begin(); mapIterator!=wordCounts.end(); mapIterator++) {
//        Word target = (*mapIterator).first;
//        cout << target << ": ";
//        for (WordVector::iterator countIterator = mapIterator->second.begin(); countIterator!=mapIterator->second.end(); countIterator++) {
//            Word context = countIterator->first;
//            unsigned int count = countIterator->second;
//            
//            cout << context << "("<<count<<") ";
//        }
//        cout << endl;
//    }
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
    dendrogram->sample();
    
    documentIndex++;
    
    if (documentIndex > 0 && documentIndex % 10 == 0) {
        cout << documentIndex << " documents processed"<<endl;// (dendrogram likelihood: "<<dendrogram->likelihood() <<")"<<endl;
        char fname[20];
        sprintf(fname,"graph.%d.weights",documentIndex);
        std::ofstream fout(fname);
        fout << graph->toString();
        fout.close();
    }
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