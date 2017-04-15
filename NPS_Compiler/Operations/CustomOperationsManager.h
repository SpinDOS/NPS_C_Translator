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
    static bool IsFunctionExists(const char *name)
    {
        if (strcmp(name, "foo") == 0 ||
                strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 ||
                strcmp(name, "min") == 0 || strcmp(name, "max") == 0 ||
                strcmp(name, "input") == 0 || strcmp(name, "output") == 0)
            return true;
        return false;
    }
};

#endif //NPS_C_TRANSLATOR_CUSTOMOPERATIONSMANAGER_H
