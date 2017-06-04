//
// Created by Alexander on 25-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_SENTENCEPARSER_H
#define NPS_C_TRANSLATOR_SENTENCEPARSER_H

#include "../../NPS_library/collection_containers/list.h"
#include "../LexemeParsing/LexemeParser.h"
#include "../TNodes/TNode.h"
#include "../../NPS_library/collection_containers/KeyValuePair.h"
#include "../Types/TypeInfo.h"

using namespace NPS_Compiler;

class SourceCodeParser
{
    TypeList<LexemeWord> *text;
    int curPos = 0;
    bool ThrowIfEndOfFile();
    bool IsValidVarName(LexemeWord *var);
    bool IsType();
    
    ResultType* TryGetResultType();
    bool TryGetDeclaration(TSimpleLinkedList<TNode> *list);
    TFunctionDefinition *GetFunctionDefinition(ResultType *readBeforeReturnType, LexemeWord *name);

    
    TList *ParseList();
    TNode *HandleExpression(bool stopOnComma);
    bool ParseNextSentence(TSimpleLinkedList<TNode> *list, bool declarationAllowed);
    TBranch *HandleOperation(TBranch *cur, LexemeWord *word,
                             bool &hasLeft, bool &expectedRight, bool stopOnComma);
    TOperation *GetTypeCast(LexemeWord *word, bool &hasLeft, bool &expectedRight);
    TBranch *HandleFunctionCall(TBranch *cur, LexemeWord *word,
                                bool &hasLeft, bool &expectedRight);
    
    
    
    
    
    TNode *GetConditionInBrackets();
    TNode *HandleKeywordDoWhile();
    TNode *HandleKeywordWhile();
    TNode *HandleKeywordFor();
    TNode *HandleKeywordIf();
    TNode *HandleKeywordSwitch();
    TNode *HandleKeywordBreakContinue();
    TNode *HandleKeywordReturn();
public:
    SourceCodeParser(TypeList<LexemeWord> *words);
    TSimpleLinkedList<TNode>* ParseWholeText();
    bool IsEnd(){return curPos >= text->count();}
};

#endif //NPS_C_TRANSLATOR_SENTENCEPARSER_H
