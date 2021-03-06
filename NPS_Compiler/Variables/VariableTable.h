//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_VARIABLETABLE_H
#define NPS_C_TRANSLATOR_VARIABLETABLE_H

#include "../TNodes/ResultType.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../LexemeParsing/LexemeParser.h"
#include "../TNodes/TNode.h"


struct VisibilityArea
{
    VisibilityArea() : table(THashTable<ResultType>(5, 5, 0)){}
    VisibilityArea* parent = nullptr;
    THashTable<ResultType> table;
};

class VariableTable
{
    static VisibilityArea *current;
public:
    static void Init();
    static void InitializeGlobal(TSimpleLinkedList<NPS_Compiler::TNode> *globalDefinitions);
    static ResultType* GetVariableType(const char *var);
    static void AddVariable(LexemeWord *var, ResultType *type);
    static void AddFictiveVariable(const char *var, ResultType *type){current->table.put(var, type);}
    static void PushVisibilityArea();
    static void PopVisibilityArea();
};

#endif //NPS_C_TRANSLATOR_VARIABLETABLE_H
