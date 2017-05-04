#ifndef NPS_INTERPRETER_OPERATIONMANAGER_H
#define NPS_INTERPRETER_OPERATIONMANAGER_H

#include <cstring>
#include "../../NPS_library/InterpreterTNodeType.h"

namespace NPS_Interpreter
{
    typedef char *(*op_handler)(char *a, char *b, char *c, int size);

    class OperationManager
    {
    public:
        static void Init();
        static op_handler GetOperation(InterpreterTNodeType type);
    };
}

#endif //NPS_INTERPRETER_OPERATIONMANAGER_H
