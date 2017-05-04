#ifndef NPS_INTERPRETER_FUNCTIONMANAGER_H
#define NPS_INTERPRETER_FUNCTIONMANAGER_H


#include "../../NPS_library/collection_containers/THashTable.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../TNodes/TNode.h"
#include "../../NPS_library/collection_containers/list_adv.h"

using namespace NPS_Interpreter;

class ParametersManager {
public:
    static TSimpleLinkedList<char> global_parameters;
};

#endif //NPS_INTERPRETER_FUNCTIONMANAGER_H
