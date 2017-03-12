//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEME_PARSER_H
#define NPS_C_TRANSLATOR_LEXEME_PARSER_H

#include "../collection_containers/list_adv.h"
#include "LexemeInfo.h"
#include "LexemeError.h"

TypeList<LexemeInfo>* ParseToLexemes(const char *s, TypeList<LexemeError> &errors);
bool TryParseAsVariable(const char *s, unsigned long &length, LexemeInfo *result);
bool TryParseAsOperation(const char *s, unsigned long &length, LexemeInfo *result);
bool TryParseAsConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);

enum NumType
{
    NumChar = 1,
    NumInt,
    NumDouble,
};
struct NumConstantDescription
{
    double num;
    NumType type;
};

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
