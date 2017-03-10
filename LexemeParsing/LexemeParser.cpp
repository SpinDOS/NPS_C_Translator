//
// Created by Alexander on 07-Mar-17.
//

#include "LexemeParser.h"

TypeList<LexemeInfo>* ParseToLexemes(const char *s, TypeList<LexemeError> &errors)
{
    TypeList<LexemeInfo> *lexemes = new TypeList<LexemeInfo>;
    LexemeInfo *curLexeme = static_cast<LexemeInfo*> (Heap::get_mem(sizeof(LexemeInfo)));
    LexemeError *curError = static_cast<LexemeError*> (Heap::get_mem(sizeof(LexemeError)));
    while (true)
    {
        // trim start
        while (*s && *s == ' ' && *s == '\t')
            s++;
        if (!*s)
            break;
        
        int length = 1;
        if (TryParseAsVariable(s, length, curLexeme) ||
            TryParseAsOperation(s, length, curLexeme) ||
            TryParseAsConstant(s, length, curLexeme, curError))
        {
            lexemes->add(curLexeme);
        }
        else
        {
            if (!curError->error_start)
            {
                curError->error_start = s;
                curError->invalid_lexeme = to_string(*s);
                curError->message = "Undefined lexeme";
            }
            errors.add(curError);
            curError->error_start = nullptr;
        }
        s += length;
    }
    Heap::free_mem(curError);
    Heap::free_mem(curError);
    return lexemes;
}

//bool TryParseAsVariable(const char *s, int &length, LexemeInfo *result){return false;}
//bool TryParseAsOperation(const char *s, int &length, LexemeInfo *result){return false;}
//bool TryParseAsConstant(const char *s, int &length, LexemeInfo *result, LexemeError *error){return false;}
