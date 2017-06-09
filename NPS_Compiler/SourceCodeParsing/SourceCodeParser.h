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
    bool IsMeetType();
    ResultType* TryGetResultType();
    
    TList* ParseList();
    TNode* ParseNextSentence();
    TNode* HandleExpression(bool stopOnComma);
    TBranch* HandleOperation(TBranch *cur, LexemeWord *word,
                             bool &hasLeft, bool &expectedRight, bool stopOnComma);
    TOperation* GetTypeCast(LexemeWord *word, bool &hasLeft, bool &expectedRight);
    TBranch* HandleFunctionCall(TBranch *cur, LexemeWord *word,
                                bool &hasLeft, bool &expectedRight);
    bool GetAllTypeDeclarations(const char *currentNamespace);
    
    
    bool GetDeclaration(TSimpleLinkedList<TNode> *list, bool functionsAllowed = false);
    TFunctionDefinition* GetFunctionDefinition(ResultType *readBeforeReturnType, LexemeWord *name);
    
    TNode *GetConditionInBrackets();
    TNode *HandleKeywordDoWhile();
    TNode *HandleKeywordWhile();
    TNode *HandleKeywordFor();
    TNode *HandleKeywordIf();
    TNode *HandleKeywordSwitch();
    TNode *HandleKeywordBreakContinue();
    TNode *HandleKeywordReturn();
    TNode *HandleKeywordDelete();
public:
    SourceCodeParser(TypeList<LexemeWord> *words);
    TSimpleLinkedList<TNode>* ParseWholeText();
    bool IsEnd(){return curPos >= text->count();}
};

#endif //NPS_C_TRANSLATOR_SENTENCEPARSER_H
