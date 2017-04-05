//
// Created by Alexander on 01-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H
#define NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H

#include "../TNodes/ResultType.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

class CustomOperationsManager
{
public:
    static ResultType *GetResultOfOperation(TBranch *operation){ return nullptr;}
};

#endif //NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H
