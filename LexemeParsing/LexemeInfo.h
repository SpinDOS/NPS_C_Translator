//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEM_INFO_H
#define NPS_C_TRANSLATOR_LEXEM_INFO_H

#include "LexemeType.h"

struct LexemeInfo
{
    LexemeType type_of_lexeme;
    void *description = nullptr;
};

#endif //NPS_C_TRANSLATOR_LEXEM_INFO_H
