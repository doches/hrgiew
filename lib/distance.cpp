#include "distance.h"
#include <iostream>
#include <list>
#include <stdio.h>

// Compute (relatively) fast dot product between WordVectors, where
// a WordVector is a std::map<Word, int>
inline unsigned int __Distance_dot(PMIVector a, PMIVector b)
{   
  if (a.size() > b.size()) {
    return __Distance_dot(b,a);
  }
  
  unsigned int sum = 0;
  for (PMIVector::iterator wordIterator=a.begin(); wordIterator!=a.end(); wordIterator++) {
    sum += (unsigned int)(wordIterator->second * b[wordIterator->first]);
  }
  
  return sum;
}

// Initialise a new Distance object.
Distance::Distance()
{
    totalCounts = 0;
}

// Delete edges below a specified weight threshold.
void Distance::threshold(Graph *graph, double minKeep)
{
    std::list<Key> deleteList;
    for (std::map<Key, Edge *>::const_iterator mapIter = graph->edges.begin(); mapIter != graph->edges.end(); mapIter++) {
        Edge *edge = mapIter->second;
        Key key = mapIter->first;
        
        if (edge->weight < minKeep) {
            deleteList.push_back(mapIter->first);
        }
    }
    
    for (std::list<Key>::const_iterator deleteIter = deleteList.begin(); deleteIter != deleteList.end(); deleteIter++) {
        graph->edges.erase(*deleteIter);
    }
}

// Update the given graph with a new target word & context.
// Computes the cosine distance between the word and all other words, and
// updates the graph using new cosine distances as edge weights.
void Distance::updateGraph(Word target, Document document, Graph *graph)
{
    // Increment the count of how many times we've seen this target word
    wordCounts[target]++;
    
    // Get the map of (context words) -> (counts) for this target word
    WordVector *targetVector = wordVector[target];
    // If we don't have that map, this must be the first time we've seen the
    // target word. 
    if (targetVector == NULL) {
        // Create a mew map and save it in the list o' maps.
        targetVector = new WordVector();
        wordVector[target] = targetVector;
    }
    
    // Get the target word's PMI vector, for updating.
    PMIVector *pmiVector = pmiVectors[target];
    if (pmiVector == NULL) {
        // If it doesn't exist, create it and add it to the map of PMI vectors
        pmiVector = new PMIVector();
        pmiVectors[target] = pmiVector;
    }

    // For each word in this document context...
    for(Document::iterator docIter = document.begin(); docIter != document.end(); docIter++) {
        Word contextWord = *docIter;
        
        // ...Increment the count of how many times we've seen it with the target word...
        (*targetVector)[contextWord]++;
        // ...and how many times we've seen it overall...
        wordCounts[contextWord]++;
        // ...annd how many words we've seen, total.
        totalCounts++;
        
        // Compute the PMI between the two words
        //
        // PMI is log( (count_xy / count_y) * (count_total / count_x)
        unsigned int count_xy = (*targetVector)[contextWord];
        unsigned int count_y = wordCounts[contextWord];
        unsigned int count_x = wordCounts[target];
        unsigned int count_total = totalCounts;
        
//        double p_x = -log((double)count_x / count_total);
//        double p_y = -log((double)count_y / count_total);
        
        // Avoid divide-by-zero problems by assuming PMI is zero for
        // anything with a zero count.
        double pmi = 0.0f;
        if (count_y > 0 && count_x > 0) {
            pmi = ((double)count_xy / count_y) * (count_total / (double)count_x);
        }
//        const int epsilon = 0.000000001f;
//        if (abs(pmi-epsilon) > epsilon) {
//            pmi = log(pmi);
//            //            pmi = pmi / (p_x < p_y ? p_x : p_y);
//        } else {
//            pmi = 0.0f;
//        }
        
        (*pmiVector)[contextWord] = pmi;
    }

    // Compute the magnitude of this target word's vector
    // Vector magnitude is sqrt(v dot v)
    double sum = 0.0f;
//    printf("PMI_Vec(%u): ",target);
    for(PMIVector::iterator wordIterator=pmiVector->begin(); wordIterator!=pmiVector->end(); wordIterator++) {
        sum += pow(wordIterator->second,2);
//        printf("%u:%f ",wordIterator->first,wordIterator->second);
    }
//    printf("\n");
    double targetMagnitude = sqrt(sum);
    // Cache this magnitude so we can use it later (in future calls w/ different target words)
    magnitude[target] = targetMagnitude;

    // Compute the distance between target word and all other encountered target words
    for(WordVectorMap::iterator mapIter = wordVector.begin(); mapIter != wordVector.end(); mapIter++) {
        // Word (map key) for other target word.
        Word otherTarget = mapIter->first;

        // Don't compute similarity between target and itself. Obviously.
        if (otherTarget != target) {
            // Get the vector for the other word
            PMIVector *otherPMIVector = pmiVectors[otherTarget];
            // If the vector doesn't exist, go ahead and create it -- but it's going to be zeroed out,
            // so don't bother doing any calculations.
            if (otherPMIVector == NULL) {
                otherPMIVector = new PMIVector();
                pmiVectors[otherTarget] = otherPMIVector;
            } else {
                // We've got a vector for the target word, and a vector for the other word;
                // go ahead and compute the distance between them!
                //
                // Cosine distance is (A dot B) / (magnitude(A) * magnitude(B))
                
                sum = 0.0f;
                for(PMIVector::iterator wordIterator=otherPMIVector->begin(); wordIterator!=otherPMIVector->end(); wordIterator++) {
                    sum += pow(wordIterator->second,2);
                }
                
                double otherMagnitude = sqrt(sum);
                double dot = __Distance_dot(*pmiVector, *otherPMIVector);
                double mag = targetMagnitude * otherMagnitude;
                double cosine = dot / mag;

                if (mag <= 0.0f) {
//                    cosine = 0.0f;
                }
                graph->addEdge(target,otherTarget,cosine);
            }
        }
    }
}
