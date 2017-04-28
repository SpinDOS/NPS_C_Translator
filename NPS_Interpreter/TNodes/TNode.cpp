
#include "TNode.h"
#include "../Variables/VariableTable.h"
#include "../Functions/FunctionManager.h"

using namespace NPS_Interpreter;

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
    std::cout << "Tlist" << std::endl;
    TKeyword* keyword;
    bool parent_is_for = false;
//    if(!((keyword = dynamic_cast<TKeyword*>(parent)) && strcmp(keyword->keyword, "for") == 0)){
    VariableTable::PushVisibilityArea();
//    }else{
//        parent_is_for = true;
//    }
    for(int i = 0; i < children->count(); i++){
        children->get(i)->Exec();
    }
//    if(!parent_is_for){
    VariableTable::PopVisibilityArea();
//    }
}

