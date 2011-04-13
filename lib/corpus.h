#include <string>
#include <map>
#include <vector>

typedef std::map<std::string,unsigned int> WordMap;
typedef std::vector<unsigned int> Document;

typedef unsigned int Word;

class Corpus
{
protected:
    std::string path;
    WordMap wordmap;
    unsigned int uniqueWords;
public:
    Corpus(std::string filename);
    Word stringToIndex(const char *word, bool *isNewWord = NULL);
    void eachDocument(void (*document_callback)(Word, Document, bool));
};