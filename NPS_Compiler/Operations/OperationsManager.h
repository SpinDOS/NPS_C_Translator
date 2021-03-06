//
// Created by Alexander on 02-May-17.
//

#ifndef NPS_C_TRANSLATOR_OPERATIONSMANAGER_H
#define NPS_C_TRANSLATOR_OPERATIONSMANAGER_H

#include "../TNodes/ResultType.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

class OperationsManager
{
public:
    static ResultType *GetResultOfOperation(TOperation *operation);
    static bool ValidateCustomOperator(Func *signature, LexemeWord *lexeme);
};

#endif //NPS_C_TRANSLATOR_OPERATIONSMANAGER_H
