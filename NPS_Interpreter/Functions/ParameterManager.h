#ifndef NPS_INTERPRETER_FUNCTIONMANAGER_H
#define NPS_INTERPRETER_FUNCTIONMANAGER_H


#include "../../NPS_library/collection_containers/THashTable.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../TNodes/TNode.h"
#include "../../NPS_library/collection_containers/list_adv.h"

using namespace NPS_Interpreter;

namespace NPS_Interpreter
{
    struct Func{
        TSimpleLinkedList<TFunctionParamsGetter>* params;
        TSimpleLinkedList<TNode>* instructions;
        char* Exec();

    private:
        void AddFunctionParamsToVariableTable();
    };
}


class ParametersManager {
public:
    static Stack<char*>* global_parameters;
};

#endif //NPS_INTERPRETER_FUNCTIONMANAGER_H
