
#include "FunctionManager.h"
#include "../Variables/VariableTable.h"
#include "../Operations/OperationManager.h"

THashTable<Func>* FunctionManager::functions_table = new THashTable<Func>(5,5,5);
Stack<char*>* FunctionManager::global_parameters = new Stack<char*>();

void FunctionManager::AddFunction(char *name, Func *func)
{
    FunctionManager::functions_table->put(name, func);
}

Func* FunctionManager::GetFunction(char *name)
{
    return FunctionManager::functions_table->get(name);
}

void Func::AddFunctionParamsToVariableTable()
{
    for(int i = 0; i < params->count(); i++){
        TFunctionParamsGetter* param = params->get(i);
        OperationManager::AssignmentM(param->Exec(), FunctionManager::global_parameters->pop(), param->size);
    }
}

char* Func::Exec()
{
    VariableTable::PushVisibilityArea();
    AddFunctionParamsToVariableTable();
    for(int i = 0; i < instructions->count() /*&& VariableTable::GetVariableType("*return") == nullptr*/; i++){
        instructions->get(i)->Exec();
    }
    VariableTable::PopVisibilityArea();
    if(FunctionManager::global_parameters->count() > 0)
        return FunctionManager::global_parameters->pop();
    return 0;
}

