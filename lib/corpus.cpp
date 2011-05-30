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
#include <sstream>
#include <string>
#include <cstring>
#include "logger.h"
#include <stdlib.h>
#include <cstring>

Corpus::Corpus(std::string filename)
{
    path = filename;
    uniqueWords = 0;
    
    int result = system((std::string("wc -l < \"")+filename+std::string("\" > corpus.length")).c_str());
    if(!result) {
	    std::ifstream fin("corpus.length",std::ifstream::in);
  	  fin >> documentCount;
    	fin.close();
    }
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
        unsigned int cMaxLen = 128;
        char *cline = (char *)malloc(cMaxLen*sizeof(char));
        int lineIndex = 0;
        while (!fin.eof()) {
            std::getline(fin,line);
            while (line.size() > cMaxLen) {
                cMaxLen *= 2;
                free(cline);
                cline = (char *)malloc(cMaxLen*sizeof(char));
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

std::string Corpus::wordmapToString()
{
    std::ostringstream oss;
    for (WordMap::iterator iter = wordmap.begin(); iter != wordmap.end(); iter++) {
        oss << iter->first << "\t" << iter->second << std::endl;
    }
    
    return oss.str();
}
