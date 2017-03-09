//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEM_INFO_H
#define NPS_C_TRANSLATOR_LEXEM_INFO_H

#include "LexemeType.h"

struct LexemeInfo
{
    ~LexemeInfo()
    {
        if (description)
            Heap::free_mem(description);
    }
    LexemeType type_of_lexem;
    void *description = nullptr;
};

#endif //NPS_C_TRANSLATOR_LEXEM_INFO_H
