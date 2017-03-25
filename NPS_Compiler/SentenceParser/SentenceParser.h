//
// Created by Alexander on 25-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_SENTENCEPARSER_H
#define NPS_C_TRANSLATOR_SENTENCEPARSER_H

#include "../../NPS_library/collection_containers/list.h"
#include "../LexemeParsing/LexemeParser.h"

void GetNextSentence(TypeList<LexemeWord> words, unsigned int&start, unsigned int &end);

#endif //NPS_C_TRANSLATOR_SENTENCEPARSER_H
