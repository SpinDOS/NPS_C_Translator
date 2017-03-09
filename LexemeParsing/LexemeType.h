//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEM_TYPES_H
#define NPS_C_TRANSLATOR_LEXEM_TYPES_H

enum LexemeType : char
{
    Keyword = 1,
    VariableName,
    CharConstant,
    StringConstant,
    NumConstant,
    Operation,
};

#endif //NPS_C_TRANSLATOR_LEXEM_TYPES_H
