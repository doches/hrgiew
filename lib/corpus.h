#ifndef __CORPUS_H__
#define __CORPUS_H__

#include <string>
#include <map>
#include <vector>

typedef std::map<std::string,unsigned int> WordMap;
typedef std::map<unsigned int, std::string> ReverseWordMap;
typedef std::vector<unsigned int> Document;

typedef unsigned int Word;

class Corpus
{
protected:
    std::string path;
    WordMap wordmap;
    ReverseWordMap reverseWordmap;
    unsigned int documentCount;
    
    unsigned int uniqueWords;
public:
    Corpus(std::string filename);
    Word stringToIndex(const char *word, bool *isNewWord = NULL);
    std::string indexToString(Word word);
    void eachDocument(void (*document_callback)(Word, Document, bool));
    unsigned int size();
};

#endif