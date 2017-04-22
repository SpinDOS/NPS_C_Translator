//
// Created by Alexander on 18-Apr-17.
//

#include <cstdio>
#include "TypeCastManager.h"
#include "../../NPS_library/collection_containers/KeyValuePair.h"
#include "../Types/TypesManager.h"

TSimpleLinkedList<KeyValuePair<ResultType, ResultType>> implicitCasts;
TSimpleLinkedList<KeyValuePair<ResultType, ResultType>> explicitCasts;

ResultType *_bool;
ResultType *_char;
ResultType *_int;
ResultType *_double;

void TypeCastManager::Init()
{
    _bool = TypesManager::GetResultType("bool");
    _char = TypesManager::GetResultType("char");
    _int = TypesManager::GetResultType("int");
    _double = TypesManager::GetResultType("double");
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_char, _bool));
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_int, _bool));
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_double, _bool));
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_int, _char));
    explicitCasts.add(new KeyValuePair<ResultType, ResultType>(_double, _char));
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_char, _int));
    explicitCasts.add(new KeyValuePair<ResultType, ResultType>(_double, _int));
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_char, _double));
    implicitCasts.add(new KeyValuePair<ResultType, ResultType>(_int, _double));
}

bool TypeCastManager::CanCast(TNode *node, ResultType *targetType, bool explicitCast)
{
    // for base and derived class this logic is another
    if (node->getType()->p_count > 0 || targetType->p_count > 0)
    {
        if (*targetType == *_bool)
            return true;
        else
            return node->getType()->p_count == targetType->p_count;
    }

    if (explicitCast)
    {
        for (int i = 0; i < explicitCasts.count(); ++i)
        {
            KeyValuePair<ResultType, ResultType> *cast = explicitCasts.get(i);
            if (*cast->key == *node->getType() && *cast->value == *targetType)
                return true;
        }
    }
    for (int i = 0; i < implicitCasts.count(); ++i)
    {
        KeyValuePair<ResultType, ResultType> *cast = implicitCasts.get(i);
        if (*cast->key == *node->getType() && *cast->value == *targetType)
            return true;
    }
    return false;
}

NPS_Interpreter::InterpreterTNodeType getPrimitiveCast(ResultType *from, ResultType *to)
{
    if (to->p_count == from->p_count)
        return NPS_Interpreter::InterpreterTNodeType::CastPointerToPointer;
    if (from->p_count > 0)
    {
        if (*to == *_bool)
            return NPS_Interpreter::InterpreterTNodeType::CastPointerToBool;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }

    if (*to == *_bool)
    {
        if (*from == *_char)
            return NPS_Interpreter::InterpreterTNodeType::CastCharToBool;
        if (*from == *_int)
            return NPS_Interpreter::InterpreterTNodeType::CastIntToBool;
        if (*from == *_double)
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToBool;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *_char)
    {
        if (*from == *_int)
            return NPS_Interpreter::InterpreterTNodeType::CastIntToChar;
        if (*from == *_double)
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToChar;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *_int)
    {
        if (*from == *_char)
            return NPS_Interpreter::InterpreterTNodeType::CastCharToInt;
        if (*from == *_double)
            return NPS_Interpreter::InterpreterTNodeType::CastDoubleToInt;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    if (*to == *_double)
    {
        if (*from == *_char)
            return NPS_Interpreter::InterpreterTNodeType::CastCharToDouble;
        if (*from == *_int)
            return NPS_Interpreter::InterpreterTNodeType::CastIntToDouble;
        return NPS_Interpreter::InterpreterTNodeType::NotDefined;
    }
    return NPS_Interpreter::InterpreterTNodeType::NotDefined;
}

void TypeCastManager::Cast(TNode *node, ResultType *targetType, bool explicitCast)
{
    if (!CanCast(node, targetType, explicitCast))
        return;
    TBranch *newNode;
    // handle here cast derived to base
    NPS_Interpreter::InterpreterTNodeType castType = getPrimitiveCast(node->getType(), targetType);
    // primitive types cast
    if (castType != NPS_Interpreter::InterpreterTNodeType::NotDefined)
    {
        TTypeCast *cast = new TTypeCast(targetType, nullptr);
        cast->intepreterTNodeType = castType;
        newNode = cast;
    }
    else // custom types cast
    {
        TFunction *functionBrackets = new TFunction(nullptr);
        functionBrackets->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::FunctionCall;
        bool fountExplicit = false;
        if (explicitCast)
            for (int i = 0; !fountExplicit && i < explicitCasts.count(); i++)
            {
                KeyValuePair<ResultType, ResultType> *pair = explicitCasts.get(i);
                if (*pair->key == *node->getType() && *pair->value == *targetType)
                    fountExplicit = true;
            }
        LexemeWord *functionName = new LexemeWord;
        string str = string(fountExplicit? "explicit" : "implicit") + " " + targetType->toString() ;
        functionName->lexeme = copy_string(str.c_str());

        TVariable *function = new TVariable(functionName);
        function->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Variable;
        function->parent = functionBrackets;
        functionBrackets->function = function;

        newNode = functionBrackets;
    }

    int index;
    for (index = 0; node->parent->children.get(index) != node; index++)
    { }
    newNode->parent = node->parent;
    newNode->children.add(node->parent->children.take(index));
    node->parent->children.insertBefore(newNode, index);
    node->parent = newNode;
}