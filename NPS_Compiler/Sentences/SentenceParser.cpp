//
// Created by Alexander on 25-Mar-17.
//

#include "SentenceParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

bool SentenceParser::GetNextSentence(TSimpleLinkedList<LexemeWord*> &sentence) // int a = foo(b,c);
{
    int max = text->count();
    if (curPos >= max)
        return false;
    LexemeWord *word;
    if (lastType != nullptr)
        sentence.add(lastType);
    else
    {
        word = text->getTyped(curPos++);
        sentence.add(word);
        if (TypesManager::GetTypeInfo(*word))
            lastType = word;
    }
    while (curPos < max)
    {
        word = text->getTyped(curPos++);
        sentence.add(word);
        if (word->code == 243) // ;
        {
            lastType = nullptr;
            return true;
        }
        else if (word->code == 242 && lastType != nullptr) // ,
        {
            return true;
        }
    }
    ReportError(word->start + word->length, "Unexpected end of file");
    return false;
}



TNode *SentenceParser::ParseNextSentence()
{
    if (ErrorReported())
        return nullptr;
    
    TSimpleLinkedList<LexemeWord*> sentence;
    if (!GetNextSentence(sentence))
        return nullptr;
    int i = sentence.count();
    TBranch *root = new TBranch;
    root->Priority = -10000;
    root->IsLeftAssociated = false;
    root->parent = nullptr;
    // do something
    return root->children.getFirst();
}

