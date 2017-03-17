//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSER_H
#define NPS_C_TRANSLATOR_LEXEMEPARSER_H

#define CHARRANGESCOUNT 80 // for patterns A - z

#include "../collection_containers/list_adv.h"
#include "LexemeParseStatesManager.h"

struct LexemeWord
{
    int code;
    const char *start;
    unsigned long length;
};

struct LexemeError
{
    int errorCode;
    const char *lexemeStart;
    const char *errorStart;
    int lexemeCode;
};

class LexemeParser
{
    CharsRange* charRanges[CHARRANGESCOUNT];
    LexemeStatesManager statesManager;
public:
    LexemeParser(const char *filename = "LexemeParsingInstructions.ini");
    virtual ~LexemeParser();
    bool ParseToLexemes(const char *fileContent, TypeList<LexemeWord> &words, LexemeError *error);
};

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
