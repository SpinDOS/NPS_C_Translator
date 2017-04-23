//
// Created by Alexander on 18-Apr-17.
//

#include <cstdio>
#include "TypeCastManager.h"
#include "../../NPS_library/collection_containers/KeyValuePair.h"
#include "../Types/TypesManager.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Variables/VariableTable.h"

bool FindTypeCast(ResultType *from, ResultType *to, bool explicitCast, char *nameBuffer)
{
    if (explicitCast)
        for (int i = 0; ; i++)
        {
            string name = string("explicit№") + to_string(i);
            ResultType *function = VariableTable::GetVariableType(name.c_str());
            if (function == nullptr)
                break;
            Func *signature = static_cast<Func *>(function->baseType);
            if (*signature->returnValue == *to && *signature->parameters.getFirst() == *from)
            {
                if (nameBuffer != nullptr)
                    memcpy(nameBuffer, name.c_str(), name.length() + 1);
                return true;
            }
        }
    
    for (int i = 0; ; i++)
    {
        string name = string("implicit№") + to_string(i);
        ResultType *function = VariableTable::GetVariableType(name.c_str());
        if (function == nullptr)
            break;
        Func *signature = static_cast<Func*>(function->baseType);
        if (*signature->returnValue == *to && *signature->parameters.getFirst() == *from)
        {
            if (nameBuffer != nullptr)
                memcpy(nameBuffer, name.c_str(), name.length() + 1);
            return true;
        }
    }
    return false;
}

NPS_Interpreter::InterpreterTNodeType getPrimitiveCast(ResultType *from, ResultType *to)
{
    if (to->p_count == from->p_count)
        return NPS_Interpreter::InterpreterTNodeType::CastPointerToPointer;
    if (from->p_count > 0)
    {
        if (*to == *TypesManager::Bool())
            return NPS_Interpreter::InterpreterTNodeType::CastPointerToBool;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    
    if (*to == *TypesManager::Bool())
    {
        if (*from == *TypesManager::Char())
            return NPS_Interpreter::InterpreterTNodeType::CastCharToBool;
        if (*from == *TypesManager::Int())
            return NPS_Interpreter::InterpreterTNodeType::CastIntToBool;
        if (*from == *TypesManager::Double())
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToBool;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *TypesManager::Char())
    {
        if (*from == *TypesManager::Int())
            return NPS_Interpreter::InterpreterTNodeType::CastIntToChar;
        if (*from == *TypesManager::Double())
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToChar;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *TypesManager::Int())
    {
        if (*from == *TypesManager::Char())
            return NPS_Interpreter::InterpreterTNodeType::CastCharToInt;
        if (*from == *TypesManager::Double())
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToInt;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *TypesManager::Double())
    {
        if (*from == *TypesManager::Char())
            return NPS_Interpreter::InterpreterTNodeType::CastCharToDouble;
        if (*from == *TypesManager::Int())
            return NPS_Interpreter::InterpreterTNodeType::CastIntToDouble;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    return NPS_Interpreter::InterpreterTNodeType::NotDefined;
}

bool TypeCastManager::ValidateCustomCast(Func *signature, LexemeWord *lexeme, bool explicitCast)
{
    if (signature->parameters.count() != 1)
    {
        ReportError(lexeme, "Invalid cast function signature (to source type)");
        return false;
    }
    ResultType *from = signature->parameters.getFirst(),
            *to = signature->returnValue;
    if (*to == *TypesManager::Void())
    {
        ReportError(lexeme, "Can not cast to void");
        return false;
    }
    if (from->p_count != 0)
    {
        ReportError(lexeme, "Can not create cast from pointer");
        return false;
    }
    int backup = to->p_count;
    to->p_count = 0;
    if (TypesManager::IsPrimitive(from) && TypesManager::IsPrimitive(to))
    {
        ReportError(lexeme, "Can not define cast between primitives");
        to->p_count = backup;
        return false;
    }
    to->p_count = backup;
    if (CanCast(from, to, explicitCast))
    {
        ReportError(lexeme, "This cast is already defined");
        return false;
    }
    if ((to->baseType->typeOfType == PrimCustFunc::Function ||
            from->baseType->typeOfType == PrimCustFunc::Function) &&
            TypesManager::IsPrimitive(from))
    {
        ReportError(lexeme, "Can not create this cast to function");
        return false;
    }
    return true;
}

bool TypeCastManager::CanCast(ResultType *from, ResultType *to, bool explicitCast)
{
    if (from->p_count > 0 && to->p_count > 0)
        return true;
    
    if (from->p_count > 0 && *to == *TypesManager::Bool())
        return true;
    
    if (from->p_count > 0)
        return false;
    
    if (*to == *TypesManager::Void() || *from == *TypesManager::Void())
        return false;

    if(TypesManager::IsPrimitive(from) && TypesManager::IsPrimitive(to))
        return true;
    
    return FindTypeCast(from, to, explicitCast, nullptr);
}

void TypeCastManager::Cast(TNode *node, ResultType *targetType, bool explicitCast)
{
    TBranch *newNode = nullptr;
    // handle here cast derived to base
    NPS_Interpreter::InterpreterTNodeType castType = getPrimitiveCast(node->getType(), targetType);
    // primitive types cast
    if (castType != NPS_Interpreter::InterpreterTNodeType::NotDefined)
    {
        if (node->parent->tNodeType == TNodeTypeCast)
        {
            node->parent->intepreterTNodeType = castType;
            return;
        }
        TTypeCast *cast = new TTypeCast(targetType, nullptr);
        cast->intepreterTNodeType = castType;
        newNode = cast;
    }
    else // custom types cast
    {
        char name[128];
        if (FindTypeCast(node->getType(), targetType, explicitCast, name))
        {
            TFunction *functionBrackets = new TFunction(nullptr);
            
            LexemeWord *functionName = new LexemeWord;
            functionName->lexeme = copy_string(name);
            
            TVariable *function = new TVariable(functionName);
            function->parent = functionBrackets;
            functionBrackets->function = function;
            
            newNode = functionBrackets;
        }
    }
    
    if (newNode == nullptr)
    {
        string error = string("Can not cast ") + node->getType()->toString() +
                string(" to ") + targetType->toString();
        ReportError(node->lexeme, error.c_str());
        return;
    }

    int index;
    for (index = 0; node->parent->children.get(index) != node; index++)
    { }
    newNode->parent = node->parent;
    newNode->children.add(node->parent->children.take(index));
    node->parent->children.insertBefore(newNode, index);
    node->parent = newNode;
}