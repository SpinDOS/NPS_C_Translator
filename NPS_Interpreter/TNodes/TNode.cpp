
#include "TNode.h"
#include "../Variables/VariableTable.h"
#include "../Functions/FunctionManager.h"

using namespace NPS_Interpreter;

bool ParentBranchIsNotFor(TBranch* branch){
    TKeyword* keyword = dynamic_cast<TKeyword*>(branch);
    return !(keyword != nullptr && strcmp(keyword->keyword, "for") == 0);
}

char* TDeclaration::Exec() {
    VariableTable::AddVariable(key, size);
    return VariableTable::GetVariableType(key);
}

char* TVariable::Exec(){
    return VariableTable::GetVariableType(key);
}

char* TFunctionDefinition::Exec(){
    Func* func = new Func();
    int params_count = 0;
    for(int i = 0; i < children->count(); i++)
    {
        if(TFunctionParamsGetter* param = dynamic_cast<TFunctionParamsGetter*>(children->get(i))){
            func->params->add(param);
            params_count++;
        }
        else{
            break;
        }
    }
    for(int i = params_count; i < children->count(); i++){
        func->instructions->add(children->get(i));
    }
    FunctionManager::functions_table->put(name, func);
}

char* TList::Exec() {
    bool flag = ParentBranchIsNotFor(parent);
    if(flag)
        VariableTable::PushVisibilityArea();
    for (int i = 0;
         i < children->count() &&
         *(bool *) VariableTable::GetVariableType("*break") == false &&
         *(bool *) VariableTable::GetVariableType("*continue") == false;
         i++) {
        children->get(i)->Exec();
    }
    if(flag){
       VariableTable::PopVisibilityArea();
    }
}



