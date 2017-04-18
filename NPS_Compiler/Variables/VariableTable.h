//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_VARIABLETABLE_H
#define NPS_C_TRANSLATOR_VARIABLETABLE_H

#include "../TNodes/ResultType.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../LexemeParsing/LexemeParser.h"

struct VisibilityArea
{
    VisibilityArea() : table(THashTable<ResultType>(5, 5, 5)){}
    VisibilityArea* parent = nullptr;
    THashTable<ResultType> table;
};

class VariableTable
{
    static VisibilityArea *current;
public:
    static ResultType* GetVariableType(LexemeWord *var);
    static void AddVariable(const char *var, ResultType *type);
    static void PushVisibilityArea();
    static void PopVisibilityArea();
};

#endif //NPS_C_TRANSLATOR_VARIABLETABLE_H
