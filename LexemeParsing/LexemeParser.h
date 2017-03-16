//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSER_H
#define NPS_C_TRANSLATOR_LEXEMEPARSER_H

#include "../collection_containers/list_adv.h"

void Init();
List* ParseToLexemes(const char *file_content, void *error);

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
