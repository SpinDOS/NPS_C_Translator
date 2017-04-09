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
    operator char* () const;
};

class LexemeParser
{
    CharsRange* charRanges[CHARRANGESCOUNT];
    LexemeStatesManager statesManager;
public:
    LexemeParser(const char *instructions);
    virtual ~LexemeParser();
    void ParseToLexemes(const char *fileContent, TypeList<LexemeWord> &words);
};

char parse_char_constant(LexemeWord &word);
char* parse_string_constant(LexemeWord &word); // free this memory
double parse_num_constant(LexemeWord &word, NumConstantType &type);
bool parse_bool_constant(LexemeWord &word);

const char *get_temp_lexemeWord_content(LexemeWord *lexeme);

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
