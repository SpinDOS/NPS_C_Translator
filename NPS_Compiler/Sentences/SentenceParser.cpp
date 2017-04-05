//
// Created by Alexander on 25-Mar-17.
//

#include "SentenceParser.h"

TSimpleLinkedList<LexemeWord*> result;

TSimpleLinkedList<LexemeWord*>* GetNextSentence(TypeList<LexemeWord> &words, unsigned int&start, unsigned int &end)
{
    result.clear();
    if (end >= words.count())
        return nullptr;
    end = start + 1;
    while (end < words.count() && words.getTyped(end)->code != 243) // 243 = ;
        result.add(words.getTyped(end++));
    return &result;
}
