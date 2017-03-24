//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSER_H
#define NPS_C_TRANSLATOR_LEXEMEPARSER_H

#define CHARRANGESCOUNT 80 // for patterns A - z

#include "../../NPS_library/collection_containers/list_adv.h"
#include "LexemeParseStatesManager.h"

enum NumConstantType
{
    CharConstant = 1,
    IntConstant,
    DoubleConstant,
};

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
    LexemeParser(const char *instructions);
    virtual ~LexemeParser();
    bool ParseToLexemes(const char *fileContent, TypeList<LexemeWord> &words, LexemeError &error);
};

char parse_char_constant(LexemeWord &word, LexemeError &error);
char* parse_string_constant(LexemeWord &word); // free this memory
double parse_num_constant(LexemeWord &word, NumConstantType &type, LexemeError &error);

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
