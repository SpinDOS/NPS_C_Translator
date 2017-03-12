//
// Created by murchan on 3/12/17.
//
#include "LexemeParser.h"
#include "../utils/string_utils.h"

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
        result -> type_of_lexeme = VariableName;
        result -> description = copy_string(start, length);
        return true;
    }
    else
        return false;
}