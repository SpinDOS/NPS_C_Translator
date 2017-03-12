//
// Created by murchan on 3/12/17.
//
#include "LexemeParser.h"
#include "../utils/string_utils.h"
#include "Keywords.h"

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
        char *currentWord = copy_string(start, length);
        int i = 0, j = 0;
        while(compare_strings(currentWord, keywords[i][j])){
            i++;
            j++;
        }
        if (i + j < 77){
            result -> type_of_lexeme = Keyword;
            result -> description = (int *) (i + j);
            return true;
        }
        else{
            result -> type_of_lexeme = VariableName;
            result -> description = currentWord;
            return true;
        }
    }
    else
        return false;
}