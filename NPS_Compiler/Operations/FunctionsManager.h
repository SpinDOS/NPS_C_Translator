//
// Created by Alexander on 01-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H
#define NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H

#include "../TNodes/ResultType.h"
#include "../TNodes/TNode.h"
#include "../LexemeParsing/LexemeParser.h"
#include "../../NPS_library/collection_containers/KeyValuePair.h"
#include "FunctionDefinition.h"

using namespace NPS_Compiler;

class FunctionsManager
{
public:
    static void AddFunction(FunctionDefinition *function);
    static ResultType *GetResultOfOperation(TBranch *operation){ return nullptr;}
    static bool IsFunctionExists(const char *name);
};

#endif //NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H
