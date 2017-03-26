//
// Created by Alexander on 25-Mar-17.
//

#include "SentenceParser.h"

bool GetNextSentence(TypeList<LexemeWord> &words, unsigned int&start, unsigned int &end)
{
    if (end >= words.count())
        return false;
    end = start + 1;
    while (end < words.count() && words.getTyped(end)->code != 243) // 243 = ;
        end++;
    return true;
}
