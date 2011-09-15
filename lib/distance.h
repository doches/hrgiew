#ifndef __DISTANCE_H__
#define __DISTANCE_H__

#include <math.h>
#include <map>
#include "corpus.h"
#include "graph.h"
#include <string>

typedef std::map<Word, unsigned int> WordVector;
typedef std::map<Word, WordVector *> WordVectorMap;
typedef std::map<Word, double> MagnitudeMap;
typedef std::map<Word, double> PMIVector;
typedef std::map<Word, PMIVector *> PMIVectorMap;

class Distance
{
public:
    Distance();
    void updateGraph(Word target, Document document, Graph *graph);
    void threshold(Graph *graph, double minKeep);
protected:
    MagnitudeMap magnitude;
    WordVectorMap wordVector;
    WordVector wordCounts;
    PMIVectorMap pmiVectors;
    unsigned int totalCounts;
};

#endif
