//
// Created by Alexander on 04-May-17.
//

#include "iostream"
#include "OperationManager.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;
using namespace std;

struct TOpAssign : TOperation
{
    ReturnResult Exec() final
    {
        ReturnResult left = this->children.getFirst()->Exec();
        ReturnResult right = this->children.getLast()->Exec();
        memcpy(left.data,right.data, size);
        right.FreeIfNeed();
        return left;
    }
};

TOperation* OperationManager::GetTOperation(InterpreterTNodeType type)
{
    switch (type)
    {
        case InterpreterTNodeType::KeywordDoWhile:
        case InterpreterTNodeType::KeywordWhile:
        case InterpreterTNodeType::KeywordFor:
        case InterpreterTNodeType::KeywordIf:
        case InterpreterTNodeType::KeywordSwitch:
        case InterpreterTNodeType::KeywordBreak:
        case InterpreterTNodeType::KeywordContinue:
        case InterpreterTNodeType::KeywordReturn:
            return GetTKeyword(type);
        case InterpreterTNodeType::Assignment:
            return new TOpAssign;
    }
    return nullptr;
}




