#ifndef NPS_INTERPRETER_OPERATIONMANAGER_H
#define NPS_INTERPRETER_OPERATIONMANAGER_H

#include "../../NPS_library/InterpreterTNodeType.h"
#include "../TNodes/TNode.h"

namespace NPS_Interpreter
{
    class OperationManager
    {
    public:
        static void Init();
        static TOperation* GetTOperation(InterpreterTNodeType type);
        static TOperation* GetTKeyword(InterpreterTNodeType type);
    };
}

#endif //NPS_INTERPRETER_OPERATIONMANAGER_H
