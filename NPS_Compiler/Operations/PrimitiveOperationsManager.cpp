//
// Created by Alexander on 01-Apr-17.
//

#include <iostream>
#include "PrimitiveOperationsManager.h"

ResultType *PrimitiveOperationsManager::GetResultOfOperation(TBranch *operation)
{
    int codeOfOperation = operation->lexeme->code;
    switch(codeOfOperation)
    {
        case 202: //++
            return nps_increment(operation);
        case 203: //--
            return nps_decrement(operation);
        case 214: //!
            return nps_logicComplement(operation);
        case 215: //~
            return nps_bitwiseComplement(operation);
        case 218: //*
            if (operation->children.count() == 1) // indirection
                return nps_indirection(operation);
            else // multiplication
                return nps_multiplication(operation);
        case 219: // / - division
            return nps_division(operation);
        case 220:
            return nps_MOD(operation);
        case 221: //+
            if(operation->children.count() == 1) // unary operator
                return nps_uPlus(operation);
            else // binary operator
                return nps_bPlus(operation);
        case 222: //-
            if(operation->children.count() == 1) // unary operator
                return nps_uMinus(operation);
            else // binary operator
                return nps_bMinus(operation);
        case 223: // <<
            return nps_leftShift(operation);
        case 224: // >>
            return nps_rightShift(operation);
        case 225: // <
            return nps_less(operation);
        case 227: // >
            return nps_great(operation);
        case 229: // ==
            return nps_relationEqual(operation);
        case 233: // !=
            return nps_notEqual(operation);
        case 234: // & - ref
            if(operation->children.count() == 1)
                return nps_reference(operation);
            else // &
                return nps_bitwiseAND(operation);
        case 235:
            return nps_bitwiseXOR(operation); // or return nps_logicAND(operation);
        case 236:
            return nps_bitwiseOR(operation);
        case 238:
            return nps_logicOR(operation);
        default:
            break;
    }
    return nullptr;
}

bool PrimitiveOperationsManager::areEqualResultTypes(TSimpleLinkedList<TNode *> children)
{
    return compare_strings(children.getFirst()->getType()->GetBaseType(), children.getLast()->getType()->GetBaseType())
           & children.getFirst()->type->GetPCount() == children.getLast()->type->GetPCount()
           & children.getFirst()->type->isConst() == children.getLast()->type->isConst();
}

ResultType *PrimitiveOperationsManager::result(TBranch *operation)
{
    return new ResultType(operation->children.getFirst()->type->GetBaseType(),
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_increment(TBranch *operation)
{
    if (operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_decrement(TBranch *operation)
{
    if (operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
{
    return result(operation);
}

ResultType* PrimitiveOperationsManager::nps_bitwiseComplement(TBranch *operation)
{
    if(operation->children.getFirst()->getType()->operator!=("double"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
{
    if (operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
{
    if (operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bPlus(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bMinus(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_multiplication(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_indirection(TBranch *operation)
{
    return new ResultType(operation->children.getFirst()->type->GetBaseType(),
                          operation->children.getFirst()->type->GetPCount() - 1,
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_reference(TBranch *operation)
{
    return new ResultType(operation->children.getFirst()->type->GetBaseType(),
                          operation->children.getFirst()->type->GetPCount() + 1,
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_division(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_MOD(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("bool"))
        return result(operation);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_leftShift(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("double") &&
        operation->children.getFirst()->type->operator!=("bool"))
        return result(operation); // нужно ли менять isConst? - подумать
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_rightShift(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("double") &&
        operation->children.getFirst()->type->operator!=("bool"))
        return result(operation); // нужно ли менять isConst? - подумать
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_less(TBranch *operation)
{
    return new ResultType("bool",
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_great(TBranch *operation)
{
    return new ResultType("bool",
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_relationEqual(TBranch *operation)
{
    return new ResultType("bool",
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_notEqual(TBranch *operation)
{
    return new ResultType("bool",
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_bitwiseAND(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("double") &&
        operation->children.getFirst()->type->operator!=("bool"))
        return result(operation); // нужно ли менять isConst? - подумать
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseXOR(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("double") &&
        operation->children.getFirst()->type->operator!=("bool"))
        return result(operation); // нужно ли менять isConst? - подумать
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseOR(TBranch *operation)
{
    if (areEqualResultTypes(operation->children) &&
        operation->children.getFirst()->getType()->operator!=("double") &&
        operation->children.getFirst()->type->operator!=("bool"))
        return result(operation); // нужно ли менять isConst? - подумать
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_logicAND(TBranch *operation)
{
    return new ResultType("bool",
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_logicOR(TBranch *operation)
{
    return new ResultType("bool",
                          operation->children.getFirst()->type->GetPCount(),
                          operation->children.getFirst()->type->isConst());
}

ResultType* PrimitiveOperationsManager::nps_ternaryOperator(TBranch *operation)
{
//    if (operation->children.get(0)->getType()->operator==("bool"))
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_dot(TBranch *operation)
{
    if (areEqualResultTypes(operation->children))
        return result(operation);
}