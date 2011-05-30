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

class Distance
{
public:
  Distance();
  void updateGraph(Word target, Document document, Graph *graph);
protected:
  MagnitudeMap magnitude;
  WordVectorMap wordVector;
};

#endif
