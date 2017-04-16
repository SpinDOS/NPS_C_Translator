//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSER_H
#define NPS_C_TRANSLATOR_LEXEMEPARSER_H

#define CHARRANGESCOUNT 80 // for patterns A - z

#include "../../NPS_library/collection_containers/list_adv.h"
#include "LexemeParseStatesManager.h"

struct LexemeWord
{
    int code;
    long positionInTheText;
    char *lexeme;
    operator const char* () const {return lexeme;}
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
bool parse_bool_constant(LexemeWord &word);
double parse_num_constant(LexemeWord &word, char **type_buffer);

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
