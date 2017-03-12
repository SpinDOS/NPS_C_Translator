//
// Created by murchan on 3/12/17.
//
#include "LexemeInfo.h"
#include "../heap/heap.h"

bool TryParseAsKeywordOrVariable(const char *s, unsigned long &length, LexemeInfo *result)
{
    const char *start = s;
    if (*s >= '0' && *s <= '9')
        return false;
    for (; *s && (*s >= '0' && *s <= '9') ||
           *s == '_' || (*s <= 'z' &&
                         *s >= 'a') || (*s <= 'Z' &&
                                        *s >= 'A'); s++)
    {}
    length = s - start;
    if (length > 0)
    {
        char *currentWord = new char[length];
        for (int i = 0; i < length; i++)
            *(currentWord + i) = *(start + i);
        result -> type_of_lexeme = VariableName;
        result -> description = Heap::get_mem(length);
        *((char *) result -> description) = *currentWord;
        return true;
    }
    else
        return false;
}