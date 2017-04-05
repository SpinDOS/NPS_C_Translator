//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_VARIABLETABLE_H
#define NPS_C_TRANSLATOR_VARIABLETABLE_H

#include "../TNodes/ResultType.h"

class VariableTable
{
public:
    static ResultType* GetVariableType(const char *var);
    static void AddVariable(const char *var, ResultType *type);
    static void PushVisibilityArea();
    static void PopVisibilityArea();
};

#endif //NPS_C_TRANSLATOR_VARIABLETABLE_H
