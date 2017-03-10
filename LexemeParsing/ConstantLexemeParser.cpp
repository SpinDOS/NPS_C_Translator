#include "LexemeParser.h"

bool TryParseAsCharConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool isDigit (const char *s) { return *s >= '0' && *s <= '9'; }

bool TryParseAsConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    switch (*s)
    {
        case '\'':
            return TryParseAsCharConstant(s, length, result, error);
        case '"':
            return TryParseAsStringConstant(s, length, result, error);
        default:
            break;
    }
    if (*s == '.' || isDigit(s))
        return TryParseAsNumConstant(s, length, result, error);
    return false;
}

bool TryParseAsCharConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    if (s[0] == '\'' && s[1] == 'a' && s[2] == '\'')
    {
        length = 3;
        result->type_of_lexeme = CharConstant;
        result->description = Heap::get_mem(1);
        *((char *)result->description) = 'a';
        return true;
    }
    else
    {
        const char *start = s++;
        while (*s && *s != ' ')
            s++;
        length = s - start;
        error->error_start = start;
        error->invalid_lexeme = std::string(start, length);
        error->message = "Invalid char constant";
        return false;
    }
}

bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    return false;
    
}
bool TryParseAsNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    return false;
}
