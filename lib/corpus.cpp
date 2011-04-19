//
//  corpus.cpp
//  hrgiew
//
//  Created by Trevor Fountain on 4/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "corpus.h"
#include <iostream>
#include <fstream>
#include <string>
#include "logger.h"
#include <stdlib.h>

Corpus::Corpus(std::string filename)
{
    path = filename;
    uniqueWords = 0;
    
    system((std::string("wc -l < \"")+filename+std::string("\" > corpus.length")).c_str());
    
    std::ifstream fin("corpus.length",std::ifstream::in);
    fin >> documentCount;
    fin.close();
}

Word Corpus::stringToIndex(const char *word, bool *isNewWord)
{
    std::string target = std::string(word);
    WordMap::iterator foundIter = wordmap.find(target);
    if (foundIter == wordmap.end()) {
        wordmap[target] = uniqueWords++;
        foundIter = wordmap.find(target);
        reverseWordmap[uniqueWords-1] = target;
        if (isNewWord != NULL)
            *isNewWord = true;
    } else if(isNewWord != NULL) {
        *isNewWord = false;
    }
    
    return foundIter->second;
}

std::string Corpus::indexToString(Word word)
{
    return reverseWordmap[word];
}

unsigned int Corpus::size()
{
    return documentCount;
}

void Corpus::eachDocument(void (*document_callback)(Word, Document, bool))
{
    std::ifstream fin(path.c_str(),std::ifstream::in);
    if(fin.good()) {
        std::string line;
        char cline[200];
        int lineIndex = 0;
        while (!fin.eof()) {
            std::getline(fin,line);
            if (line.size() > 200) {
                Log::message("Corpus","%s line %d too long (> 200 characters).",path.c_str(),lineIndex,Log::ERROR);
                exit(1);
            }
            strcpy(cline,line.c_str());
            if (line.size() > 1) {
                bool newTarget = false;
                Word target = stringToIndex(strtok(cline," "),&newTarget);
                
                Document doc;
                char *token = strtok(NULL, " ");
                while (token != NULL) {
                    doc.push_back(stringToIndex(token));
                    token = strtok(NULL, " ");
                }
                
                document_callback(target,doc,newTarget);
            }
            lineIndex++;
        }
        fin.close();
    }
}