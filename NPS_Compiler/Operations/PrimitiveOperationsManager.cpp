//
// Created by Alexander on 01-Apr-17.
//

#include <iostream>
#include "PrimitiveOperationsManager.h"

ResultType *PrimitiveOperationsManager::NPS_BOOL = new ResultType("bool");
ResultType *PrimitiveOperationsManager::NPS_CHAR = new ResultType("char");
ResultType *PrimitiveOperationsManager::NPS_DOUBLE = new ResultType("double");
ResultType *PrimitiveOperationsManager::NPS_INT = new ResultType("int");

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
            if (operation->NumOfChildren == 1) // indirection
                return nps_indirection(operation);
            else // multiplication
                return nps_multiplication(operation);
        case 219: // / - division
            return nps_division(operation);
        case 220: // %
            return nps_MOD(operation);
        case 221: //+
            if(operation->NumOfChildren == 1) // unary operator
                return nps_uPlus(operation);
            else // binary operator
                return nps_bPlus(operation);
        case 222: //-
            if(operation->NumOfChildren == 1) // unary operator
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
            if(operation->NumOfChildren == 1)
                return nps_reference(operation);
            else // &
                return nps_bitwiseAND(operation);
        case 235: // &&
            return  nps_logicAND(operation);
        case 236: // |
            return nps_bitwiseOR(operation);
        case 237: // ^
            return nps_bitwiseXOR(operation);
        case 238: // &&
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
    ResultType *operand = operation->children.getFirst()->getType();
}

ResultType* PrimitiveOperationsManager::nps_decrement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
}

ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
}

ResultType* PrimitiveOperationsManager::nps_bitwiseComplement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
}

ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
}

ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
}

ResultType* PrimitiveOperationsManager::nps_bPlus(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_bMinus(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_multiplication(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
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
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_MOD(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_leftShift(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_rightShift(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_less(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_great(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_relationEqual(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_notEqual(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_bitwiseAND(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_bitwiseXOR(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_bitwiseOR(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_logicAND(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_logicOR(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}

ResultType* PrimitiveOperationsManager::nps_ternaryOperator(TBranch *operation)
{
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_comma(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
}