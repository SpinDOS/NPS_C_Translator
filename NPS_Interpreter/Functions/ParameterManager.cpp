
#include "ParameterManager.h"
#include "../Variables/VariableTable.h"
#include "../Operations/OperationManager.h"

Stack<char*>* ParametersManager::global_parameters = new Stack<char*>();

void Func::AddFunctionParamsToVariableTable()
{
    VariableTable::AddVariable("*return", sizeof(bool));
    *(bool*)VariableTable::GetVariableType("*return") = false;
    for(int i = 0; i < params->count(); i++){
        TFunctionParamsGetter* param = params->get(i);
        OperationManager::AssignmentM(param->Exec(), ParametersManager::global_parameters->pop(), param->size);
    }
}

char* Func::Exec()
{
    VariableTable::PushVisibilityArea();
    AddFunctionParamsToVariableTable();
    for(int i = 0;
        i < instructions->count() &&
        *(bool*)VariableTable::GetVariableType("*return") == false;
        i++){
        instructions->get(i)->Exec();
    }
    VariableTable::PopVisibilityArea();
    return 0;
}

