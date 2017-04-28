#ifndef NPS_INTERPRETER_FUNCTIONMANAGER_H
#define NPS_INTERPRETER_FUNCTIONMANAGER_H


#include "../../NPS_library/collection_containers/THashTable.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../TNodes/TNode.h"
#include "../../NPS_library/collection_containers/list_adv.h"

using namespace NPS_Interpreter;

struct Func{
    Func(){
        params = new TSimpleLinkedList<TFunctionParamsGetter>();
        instructions = new TSimpleLinkedList<TNode>();
    }
    TSimpleLinkedList<TFunctionParamsGetter>* params;
    TSimpleLinkedList<TNode>* instructions;
    char* Exec();

private:
    void AddFunctionParamsToVariableTable();
};

class FunctionManager {
public:
    static THashTable<Func>* functions_table;
    static Stack<char*>* global_parameters;
    static void AddFunction(char* name, Func* func);
    static Func* GetFunction(char* name);
};

#endif //NPS_INTERPRETER_FUNCTIONMANAGER_H
