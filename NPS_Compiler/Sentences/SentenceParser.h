//
// Created by Alexander on 25-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_SENTENCEPARSER_H
#define NPS_C_TRANSLATOR_SENTENCEPARSER_H

#include "../../NPS_library/collection_containers/list.h"
#include "../LexemeParsing/LexemeParser.h"
#include "../TNodes/TNode.h"
#include "../Types/TypeInfo.h"

using namespace NPS_Compiler;

class SentenceParser
{
    TypeList<LexemeWord> *text;
    int curPos = 0;
    TBranch *HandleOperation(TBranch *cur, LexemeWord *word,
                             bool &hasLeft, bool &expectedRight, bool stopOnComma);
    TBranch *HandleFunctionCall(TBranch *cur, LexemeWord *word,
                             bool &hasLeft, bool &expectedRight);
    TNode *HandleExpression(bool stopOnComma);
    TNode *HandleDeclaration();
public:
    SentenceParser(TypeList<LexemeWord> *words);
    TNode *ParseNextSentence();
    TList *ParseList();
    bool IsEnd(){return curPos >= text->count();}
};

#endif //NPS_C_TRANSLATOR_SENTENCEPARSER_H
