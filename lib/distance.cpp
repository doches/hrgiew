#include "distance.h"
#include <stdio.h>

inline unsigned int __Distance_dot(WordVector a, WordVector b)
{   
  if (a.size() > b.size()) {
    return __Distance_dot(b,a);
  }
  
  unsigned int sum = 0;
  for (WordVector::iterator wordIterator=a.begin(); wordIterator!=a.end(); wordIterator++) {
    printf("\t%u %u\n",wordIterator->second,wordIterator->first);
    sum += wordIterator->second * b[wordIterator->first];                                
  }
  
  return sum;
}

Distance::Distance()
{
  
}

void Distance::updateGraph(Word target, Document document, Graph *graph)
{
  WordVector *targetVector = wordVector[target];
  if (targetVector == NULL) {
    targetVector = new WordVector();
    wordVector[target] = targetVector;
  }
  
  for(Document::iterator docIter = document.begin(); docIter != document.end(); docIter++)
  {
    (*targetVector)[*docIter]++;
    
  }

  printf("(%u) ",target);
  for(WordVector::iterator wordIter = targetVector->begin(); wordIter != targetVector->end(); wordIter++) {
    printf("%u:%u ",wordIter->first,wordIter->second);
  }
  printf("\n");

  double sum = 0.0f;
  for(WordVector::iterator wordIterator=targetVector->begin(); wordIterator!=targetVector->end(); wordIterator++) {
    sum += pow(wordIterator->second,2);
  }
  double targetMagnitude = sqrt(sum);
  magnitude[target] = targetMagnitude;
  
  for(WordVectorMap::iterator mapIter = wordVector.begin(); mapIter != wordVector.end(); mapIter++) {
    Word otherTarget = mapIter->first;
    
    if (otherTarget != target) {
      WordVector *otherVector = mapIter->second;
      if (otherVector == NULL) {
        otherVector = new WordVector();
        wordVector[otherTarget] = otherVector;
      }
      double otherMagnitude = magnitude[otherTarget];
      double dot = __Distance_dot(*targetVector, *otherVector);
      double mag = targetMagnitude * otherMagnitude;
      double cosine = dot / mag;
      
      if (cosine > 0) {
        graph->addEdge(target,otherTarget,cosine);
      }
//      printf("%u %u: %1.4f / %1.4f\n",target,otherTarget,dot,mag);
    }
  }
}
