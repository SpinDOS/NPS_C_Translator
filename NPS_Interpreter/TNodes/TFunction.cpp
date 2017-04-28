#include "TNode.h"
#include "../Functions/FunctionManager.h"
#include "math.h"
#include "../Operations/OperationManager.h"

#define PI 3.14159

using namespace NPS_Interpreter;

char* TFunction::Exec() {
    char* function_name = ((TVariable*)children->get(0))->key;
    if(strcmp(function_name, "min") == 0){
        return OperationManager::Min(children->get(1)->Exec(), children->get(1)->Exec());
    }
    if(strcmp(function_name, "max") == 0){
        return OperationManager::Max(children->get(1)->Exec(), children->get(1)->Exec());
    }
    if(strcmp(function_name, "output") == 0){
        OperationManager::Output(children->get(1)->Exec());
        return nullptr;
    }
    if(strcmp(function_name, "outputDouble") == 0){
        OperationManager::OutputDouble(children->get(1)->Exec());
        return nullptr;
    }
    char* result;
    if(strcmp(function_name, "sin") == 0){
        result = (char*)Heap::get_mem(sizeof(double));
        *result = sin(PI * *(double*)children->get(1)->Exec()/180);
        return result;
    }
    if(strcmp(function_name, "cos") == 0){
        result = (char*)Heap::get_mem(sizeof(double));
        *result = cos(PI * *(double*)children->get(1)->Exec()/180);
        return result;
    }
    Func* function = FunctionManager::functions_table->get(function_name);
    for(int i = children->count() - 1; i > 0; i--){
        FunctionManager::global_parameters->push(children->get(i)->Exec());
    }
    function->Exec();
    return 0; //TODO RETURN RESULT VALUE
}
