//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEME_PARSER_H
#define NPS_C_TRANSLATOR_LEXEME_PARSER_H

#include "../collection_containers/list_adv.h"
#include "LexemeInfo.h"
#include "LexemeParsingException.h"

Queue<LexemeInfo> ParseToLexemes(const char *s, TypeList<LexemeParsingException> &errors);

#endif //NPS_C_TRANSLATOR_LEXEMEPARSER_H
