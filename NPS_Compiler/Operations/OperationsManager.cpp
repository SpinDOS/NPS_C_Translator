//
// Created by Alexander on 02-May-17.
//

#include "OperationsManager.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../TypeCast/TypeCastManager.h"
#include "../Types/TypesManager.h"
#include "PrimitiveOperationsManager.h"

ResultType* reference(TOperation *operation);
ResultType* dereference(TOperation *operation);
ResultType* ternary(TOperation *operation);
ResultType* assignment(TOperation *operation);
ResultType* indexer(TOperation *operation);

ResultType* OperationsManager::GetResultOfOperation(TOperation *operation)
{
    bool custom_exists = false;
    for (int i = 0; i < operation->children.count(); i++)
    {
        ResultType *resultType = operation->children.get(i)->getType();
        if (*resultType == *TypesManager::Void())
        {
            ReportError(operation->lexeme, "Can not perform operation with void");
            return nullptr;
        }
        if (resultType->p_count == 0 && !TypesManager::IsPrimitive(resultType))
            custom_exists = true;
    }
    if (operation->children.count() == 1 && operation->lexeme->code == 234) // &
        return reference(operation);
    if (operation->children.count() == 1 && operation->lexeme->code == 218) // *
        return dereference(operation);
    if (operation->lexeme->code == 242) // ,
    {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Comma;
        return operation->children.getLast()->getType();
    }
    if (operation->lexeme->code == 240) // ?:
        return ternary(operation);
    if (operation->lexeme->code == 241) // =
        return assignment(operation);
    if (operation->lexeme->code == 206) // []
        return indexer(operation);
    if (!custom_exists)
        return PrimitiveOperationsManager::GetResultOfOperation(operation);
    else
        // here custom operationsManager
    return nullptr;
}

ResultType* reference(TOperation *operation)
{
    ResultType *operand_type = operation->children.getFirst()->getType();
    ResultType *result_operand = operand_type->clone();
    result_operand->p_count++;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Reference;
    return result_operand;
}

ResultType* dereference(TOperation *operation)
{
    ResultType *operand_type = operation->children.getFirst()->getType();
    ResultType *result_operand = operand_type->clone();
    if (result_operand->p_count == 0) {
        ReportError(operation->lexeme, "Pointer type is required");
        return nullptr;
    }
    result_operand->p_count--;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Dereference;
    return result_operand;
}

ResultType* ternary(TOperation *operation)
{
    TNode *operand1 = operation->children.get(1), *operand2 = operation->children.getLast();
    ResultType *operand1_type = operand1->getType();
    ResultType *operand2_type = operand2->getType();
    TypeCastManager::Cast(operation->children.getFirst(), TypesManager::Bool(), false);
    if (ErrorReported())
        return nullptr;
    if (TypeCastManager::CanCast(operand2, operand1_type, false))
        TypeCastManager::Cast(operation->children.getLast(), operand1_type, false);
    else
        TypeCastManager::Cast(operation->children.get(1), operand2_type, false);
    if (ErrorReported())
        return nullptr;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Ternary;
    return operation->children.getLast()->getType();
}

ResultType* assignment(TOperation *operation)
{
    TypeCastManager::Cast(operation->children.getLast(),
                          operation->children.getFirst()->getType(), false);
    if (ErrorReported())
        return nullptr;
    // FOR CLASSES COPY METHOD CAN BE HERE
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Assignment;
    return operation->children.getFirst()->getType();
}

ResultType* indexer(TOperation *operation)
{
    ResultType *base = operation->children.getFirst()->getType();
    if (base->p_count == 0)
    {
        if (TypesManager::IsPrimitive(base))
        {
            ReportError(operation->lexeme, "Can not get index of non-pointer type");
            return nullptr;
        }
        // here custom operationsManager
        return nullptr;
    }
    TypeCastManager::Cast(operation->children.getLast(), TypesManager::Int(), false);
    if (ErrorReported())
        return nullptr;
    
    LexemeWord *lexemePlus = static_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
    lexemePlus->positionInTheText = -1;
    lexemePlus->code = 221; // +
    lexemePlus->lexeme = copy_string("+");
    LexemeWord *lexemeDereference = static_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
    lexemeDereference->positionInTheText = -1;
    lexemeDereference->code = 218; // *
    lexemeDereference->lexeme = copy_string("*");
    
    bool hasLeft = true, expectedRight = false;
    TOperation *opPlus = GetTOperation(lexemePlus, hasLeft, expectedRight);
    opPlus->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusIntPointer;
    hasLeft = false; expectedRight = false;
    TOperation *opDereference = GetTOperation(lexemeDereference, hasLeft, expectedRight);
    opDereference->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Dereference;
    
    opDereference->parent = operation->parent;
    opDereference->children.add(opPlus);
    
    opPlus->parent = opDereference;
    for (int i = 0; i < 2; i++)
    {
        TNode *child = operation->children.takeFirst();
        child->parent = opPlus;
        opPlus->children.add(child);
    }
    
    int i;
    for (i = 0; i < operation->parent->children.count(); i++)
        if (operation->parent->children.get(i) == operation)
            break;
    operation->parent->children.take(i);
    operation->parent->children.insertBefore(opDereference, i);
    delete operation;
    
    base = base->clone();
    base->p_count--;
    return base;
}