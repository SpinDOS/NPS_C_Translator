//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSER_H
#define NPS_C_TRANSLATOR_LEXEMEPARSER_H

#define CHARRANGESCOUNT 80 // for patterns A - z

#include "../collection_containers/list_adv.h"
#include "LexemeParseStatesManager.h"

class LexemeParser
{
    CharsRange* charRanges[CHARRANGESCOUNT];
    LexemeStatesManager statesManager;
public:
    LexemeParser(const char *filename = "LexemeParsingPatterns.ini");
    virtual ~LexemeParser();
    bool ParseToLexemes(const char *file_content, List *lexemes, void *error);
    LexemeStatesManager* DELETETHISMETHODLATER(){ return &statesManager;}
};

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
