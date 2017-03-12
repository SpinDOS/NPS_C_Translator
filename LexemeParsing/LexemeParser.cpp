//
// Created by Alexander on 07-Mar-17.
//

#include "LexemeParser.h"

string get_lexeme_until_space(const char *s, unsigned long &length);
bool is_empty_space(const char &ch);

TypeList<LexemeInfo>* ParseToLexemes(const char *s, TypeList<LexemeError> &errors)
{
    TypeList<LexemeInfo> *lexemes = new TypeList<LexemeInfo>;
    LexemeInfo *curLexeme = static_cast<LexemeInfo*> (Heap::get_mem(sizeof(LexemeInfo)));
    LexemeError *curError = static_cast<LexemeError*> (Heap::get_mem(sizeof(LexemeError)));
    while (true)
    {
        // trim start
        while (*s && is_empty_space(*s))
            s++;
        if (!*s)
            break;
    
        unsigned long length = 0;
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
                curError->invalid_lexeme = get_lexeme_until_space(s, length);
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

bool is_empty_space(const char &ch) { return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'; }

string get_lexeme_until_space(const char *s, unsigned long &length)
{
    const char *start = s;
    while (*s && !is_empty_space(*s))
        s++;
    length = s - start;
    return string(start, length);
}

bool TryParseAsVariable(const char *s, unsigned long &length, LexemeInfo *result){return false;}
bool TryParseAsOperation(const char *s, unsigned long &length, LexemeInfo *result){return false;}
