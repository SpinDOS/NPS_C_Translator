//
// Created by Alexander on 04-May-17.
//

#include "OperationManager.h"
#include "../TNodes/TNode.h"

using namespace NPS_Interpreter;

void OperationManager::Init()
{
    TFunctionDefinition def1;
    def1.name = copy_string("max");
}

op_handler OperationManager::GetOperation(InterpreterTNodeType type)
{
    return nullptr;
}