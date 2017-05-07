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

void check_is_assignable(TNode *node);

ResultType *handleCustom(TOperation *operation);

bool OperationsManager::ValidateCustomOperator(Func *signature, LexemeWord *lexeme)
{
    bool allPrimitives = true, allPointers = true;
    for (int i = 0; i < signature->parameters.count(); i++)
        if (!TypesManager::IsPrimitive(signature->parameters.get(i)))
            allPrimitives = false;
        else if (signature->parameters.get(i)->p_count == 0)
            allPointers = false;
    if (allPrimitives || allPointers)
    {
        ReportError(lexeme, "Operator overload does not have custom type parameter");
        return false;
    }
    switch (lexeme->code)
    {
        case 234: // |
        case 236: // &
        case 237: // ^
        case 225: // <
        case 226: // <=
        case 227: // >
        case 228: // >=
        case 229: // ==
        case 233: // !=
        case 223: // <<
        case 224: // >>
        case 218: // *
        case 219: // /
        case 220: // %
            if (signature->parameters.count() == 2)
                return true;
            break;
        case 214: // !
        case 215: // ~
            if (signature->parameters.count() == 1)
                return true;
            break;
        case 202: // ++
        case 203: // --
        case 221: // +
        case 222: // -
            if (signature->parameters.count() <= 2)
                return true;
            break;
        default:
            ReportError(lexeme, "This operator overload is not supported");
            return false;
    }
    ReportError(lexeme, "Invalid parameters count");
    return false;
}

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
    {
        if (operation->lexeme->code == 202 || operation->lexeme->code == 203) // ++ --
        {
            check_is_assignable(operation->children.getFirst());
            if (ErrorReported())
                return nullptr;
        }
        return PrimitiveOperationsManager::GetResultOfOperation(operation);
    }
    else
        return handleCustom(operation);
}

ResultType* reference(TOperation *operation)
{
    check_is_assignable(operation->children.getFirst());
    if (ErrorReported())
        return nullptr;
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
    check_is_assignable(operation->children.getFirst());
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
        ReportError(operation->lexeme, "Indexer for custom types is not supported yet");
        return nullptr;
    }
    TypeCastManager::Cast(operation->children.getLast(), TypesManager::Int(), false);
    if (ErrorReported())
        return nullptr;
    
    LexemeWord *lexemePlus = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
    lexemePlus->positionInTheText = -1;
    lexemePlus->code = 221; // +
    lexemePlus->lexeme = copy_string("+");
    LexemeWord *lexemeDereference = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
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
    while (operation->children.count() > 0)
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

void check_is_assignable(TNode *node)
{
    switch (node->tNodeType)
    {
        case TNodeTypeVariable:
        case TNodeTypeDeclaration:
            return;
        case TNodeTypeOperation:
            break;
        default:
            ReportError(node->lexeme, "Expression is not assignable");
            return;
    }
    TOperation *operation = static_cast<TOperation*>(node);
    int code = operation->lexeme->code;
    if (code == 202 || code == 203) // ++ --
    {
        int child_code = operation->children.getFirst()->lexeme->code;
        if (child_code == 202 || child_code == 203) // ++ --
            ReportError(operation->lexeme, "Can not use increment or decrement twice");
        else if (operation->IsLeftAssociated)
            ReportError(operation->lexeme, "Postfix increment and decrement are not assignable");
        return;
    }
    if (code == 218 && operation->children.count() == 1) // *
        return;
    if (code != 240 && code != 242) // ?: ,
    {
        ReportError(operation->lexeme, "The result of the operation is not assignable");
        return;
    }
    for (int i = 1; !ErrorReported() && i < operation->children.count(); i++)
        check_is_assignable(operation->children.get(i));
}

ResultType *handleCustom(TOperation *operation)
{
    // create TFunction
    TFunction *tFunction = TFunction::Create_abstract_function();
    tFunction->function = new TVariable(operation->lexeme);
    tFunction->function->parent = tFunction;
    
    if (operation->IsLeftAssociated &&
            (operation->lexeme->code == 202 || operation->lexeme->code == 203)) // ++ --
    {
        LexemeWord *lexemeWord = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
        lexemeWord->code = 120; // num constant
        lexemeWord->lexeme = copy_string("0");
        lexemeWord->positionInTheText = -1;
        bool hasLeft = false, expectedRight = false;
        operation->children.add(GetTLeaf(lexemeWord, hasLeft, expectedRight));
    }
    
    while (operation->children.count() > 0)
    {
        TNode *child = operation->children.takeFirst();
        child->parent = tFunction;
        tFunction->children.add(child);
    }
    delete operation;
    return tFunction->getType();
}