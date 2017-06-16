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
            string name = string("explicit#") + to_string(i);
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
        string name = string("implicit#") + to_string(i);
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

NPS_Interpreter::InterpreterTNodeType getPrimitiveCast(ResultType *from, ResultType *to, bool explicitCast)
{
    if (
        (to->p_count > 0 && from->p_count > 0 &&
            (explicitCast ||
                (to->p_count == 1 &&
                 to->baseType->operator==(*TypesManager::Void()->baseType))
            )
        )
        ||
        (explicitCast && from->baseType->typeOfType == PrimCustFunc::Function
             && to->baseType->typeOfType == PrimCustFunc::Function)
       )
        return NPS_Interpreter::InterpreterTNodeType::CastPointerToPointer;
    if (from->p_count > 0 || from->baseType->typeOfType == PrimCustFunc::Function)
    {
        if (*to == *TypesManager::Bool())
            return NPS_Interpreter::InterpreterTNodeType::CastPointerToBool;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (to->p_count > 0 || to->baseType->typeOfType == PrimCustFunc::Function)
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    
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
        if (explicitCast && *from == *TypesManager::Double())
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToChar;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *TypesManager::Int())
    {
        if (*from == *TypesManager::Char())
            return NPS_Interpreter::InterpreterTNodeType::CastCharToInt;
        if (explicitCast && *from == *TypesManager::Double())
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

bool TypeCastManager::ValidateCustomCast(Func *signature, LexemeWord *lexeme)
{
    if (signature->parameters.count() != 1)
    {
        ReportError(lexeme, "Invalid cast function signature (source type)");
        return false;
    }
    ResultType *from = signature->parameters.getFirst(),
            *to = signature->returnValue;
    if (*to == *TypesManager::Void())
    {
        ReportError(lexeme, "Can not cast to void");
        return false;
    }
    if (to->p_count > 0 && from->p_count > 0)
    {
        ReportError(lexeme, "Can not redefine casting pointer to pointer");
        return false;
    }
    if (TypesManager::IsPrimitive(from) || TypesManager::IsPrimitive(to))
    {
        ReportError(lexeme, "Can not define cast between primitives");
        return false;
    }
    if (FindTypeCast(from, to, strcmp(lexeme->lexeme, "explicit") == 0, nullptr))
    {
        ReportError(lexeme, "This cast is already defined");
        return false;
    }
    
    return true;
}

bool TypeCastManager::CanCast(ResultType *from, ResultType *to, bool explicitCast)
{
    if (*to == *from)
        return true;
    if(getPrimitiveCast(from, to, explicitCast) != NPS_Interpreter::InterpreterTNodeType::NotDefined)
        return true;
    return FindTypeCast(from, to, explicitCast, nullptr);
}

bool isZeroToPointer(TNode *from, ResultType *to)
{
    if (from->tNodeType != TNodeTypeConstant ||
            (to->p_count == 0 && to->baseType->typeOfType != PrimCustFunc::Function))
        return false;
    TConstant *tConstant = static_cast<TConstant *>(from);
    if (tConstant->data == nullptr)
        return *tConstant->constantType == *to;
    if (tConstant->constantType == TypesManager::Char())
        return *static_cast<char *>(tConstant->data) == 0;
    if (tConstant->constantType == TypesManager::Int())
        return *static_cast<int *>(tConstant->data) == 0;
    return false;
}

bool TypeCastManager::CanCast(TNode *from, ResultType *to, bool explicitCast)
{
    if (isZeroToPointer(from, to))
        return true;
    return CanCast(from->getType(), to, explicitCast);
}

void TypeCastManager::Cast(TNode *node, ResultType *targetType, bool explicitCast)
{
    if (*node->getType() == *targetType)
        return;
    if (isZeroToPointer(node, targetType))
    {
        TConstant *tConstant = static_cast<TConstant*>(node);
        tConstant->constantType = targetType;
        if (tConstant->data)
        {
            Heap::free_mem(tConstant->data);
            tConstant->data = nullptr;
        }
        return;
    }
    TBranch *newNode = nullptr;
    // handle here cast derived to base
    NPS_Interpreter::InterpreterTNodeType castType = getPrimitiveCast(node->getType(), targetType, explicitCast);
    // primitive types cast
    if (castType != NPS_Interpreter::InterpreterTNodeType::NotDefined)
    {
        if (node->parent->tNodeType == TNodeTypeCast)
        {
            node->parent->intepreterTNodeType = castType;
            static_cast<TTypeCast*>(node->parent)->targetType = targetType;
            return;
        }
        LexemeWord *word = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
        word->code = 205; // )
        word->lexeme = copy_string("(type)");
        word->positionInTheText = -1;
        TTypeCast *cast = new TTypeCast(targetType, word);
        cast->intepreterTNodeType = castType;
        newNode = cast;
    }
    else
    { // custom types cast
        char name[128];
        if (FindTypeCast(node->getType(), targetType, explicitCast, name))
        {
            TFunction *functionBrackets = TFunction::Create_abstract_function();
            
            LexemeWord *functionName = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
            functionName->lexeme = copy_string(name);
            functionName->code = 400;
            functionName->positionInTheText = -1;
            
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