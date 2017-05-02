//
// Created by Alexander on 01-Apr-17.
//

#include "PrimitiveOperationsManager.h"
#include "../ErrorReporter/ErrorReporter.h"
#include <cstring>
#include "../Types/TypesManager.h"
#include "../TypeCast/TypeCastManager.h"

ResultType *NPS_BOOL;
ResultType *NPS_CHAR;
ResultType *NPS_DOUBLE;
ResultType *NPS_INT;

void PrimitiveOperationsManager::Init()
{
    NPS_BOOL = TypesManager::Bool();
    NPS_CHAR = TypesManager::Char();
    NPS_DOUBLE = TypesManager::Double();
    NPS_INT = TypesManager::Int();
}

void castCharToInt(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    if (*operand->getType() == *NPS_CHAR)
        TypeCastManager::Cast(operand, NPS_INT, false);
}

void castCharToAnotherOperandType(TBranch *operation)
{
    TNode *op1 = operation->children.getFirst();
    TNode *op2 = operation->children.getLast();
    ResultType *op1type = op1->getType();
    ResultType *op2type = op2->getType();
    if (*op1type == *NPS_CHAR && *op2type == *NPS_CHAR)
    {
        TypeCastManager::Cast(op1, NPS_INT, false);
        TypeCastManager::Cast(op2, NPS_INT, false);
        return;
    }
    if (*op1type == *op2type)
        return;
    if (*op1type == *NPS_CHAR)
        if (op2type->p_count > 0)
            TypeCastManager::Cast(op1, NPS_INT, false);
        else
            TypeCastManager::Cast(op1, op2type, false);
    else if (*op2type == *NPS_CHAR)
        if (op1type->p_count > 0)
            TypeCastManager::Cast(op2, NPS_INT, false);
        else
            TypeCastManager::Cast(op2, op1type, false);
}

ResultType *PrimitiveOperationsManager::GetResultOfOperation(TBranch *operation)
{
    int function_count = 0;
    for (int i = 0; i < operation->children.count(); i++)
        if (operation->children.get(i)->getType()->baseType->typeOfType == PrimCustFunc::Function)
            function_count++;
    if (function_count > 0)
    {
        if (function_count == 2)
            switch (operation->lexeme->code)
            {
                case 229: // ==
                    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualPointers;
                    return TypesManager::Bool();
                case 233: // !=
                    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualPointers;
                    return TypesManager::Bool();
            }
        ReportError(operation->lexeme, "Can not perform operation with the function");
        return nullptr;
    }
    
    switch(operation->children.count())
    {
        case 1:
            castCharToInt(operation);
            break;
        case 2:
            castCharToAnotherOperandType(operation);
        default:
            break;
    }
    switch(operation->lexeme->code)
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
            return nps_multiplication(operation);
        case 219: // / - division
            return nps_division(operation);
        case 220: // %
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
        case 226: // <=
            return nps_lessEqual(operation);
        case 227: // >
            return nps_great(operation);
        case 228: // >=
            return nps_greatEqual(operation);
        case 229: // ==
            return nps_equal(operation);
        case 233: // !=
            return nps_notEqual(operation);
        case 234: // & - bitwiseAnd
            return nps_bitwiseAND(operation);
        case 235: // &&
            return  nps_logicAND(operation);
        case 236: // |
            return nps_bitwiseOR(operation);
        case 237: // ^
            return nps_bitwiseXOR(operation);
        case 238: // ||
            return nps_logicOR(operation);
        default:
            break;
    }
    return nullptr;
}

string incompatibleTypesError(ResultType *operand1, ResultType *operand2, const char *op)
{
    return "Binary operator \'" + string(op) +
            "\' can't be applied to the expressions of type \'" +
           operand1->toString() + "\' and \'" + operand2->toString() + "\'";
}

string incompatibleTypesError(ResultType *operand, const char *op)
{
    return "Unary operator \'" + string(op) +
            "\' can't be applied to the expression of type \'" +
            operand->toString() + "\'";
}

ResultType* PrimitiveOperationsManager::nps_increment(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_BOOL) {
        ReportError(operation->lexeme, incompatibleTypesError(operand_type, "++").c_str());
        return nullptr;
    }
    if (operation->IsLeftAssociated) // postfix
    {
        if (*operand_type == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixIncInt;
        if (*operand_type == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixIncDouble;
        if (operand_type->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixIncPointer;
        return operand_type;
    }
    else // prefix
    {
        if (*operand_type == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixIncInt;
        if (*operand_type == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixIncDouble;
        if (operand_type->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixIncPointer;
        return operand_type;
    }
}

ResultType* PrimitiveOperationsManager::nps_decrement(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_BOOL) {
        ReportError(operation->lexeme, incompatibleTypesError(operand_type, "--").c_str());
        return 0;
    }
    if (operation->IsLeftAssociated) // postfix
    {
        if (*operand_type == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixDecInt;
        if (*operand_type == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixDecDouble;
        if (operand_type->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixDecPointer;
        return operand_type;
    }
    else // prefix
    {
        if (*operand_type == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixDecInt;
        if (*operand_type == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixDecDouble;
        if (operand_type->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixDecPointer;
        return operand_type;
    }
}

void check_unary_plus_minus(TBranch *operation)
{
    ResultType *type = operation->children.getFirst()->getType();
    if (*type == *NPS_BOOL)
        ReportError(operation->lexeme,
                    incompatibleTypesError(type, operation->lexeme->lexeme).c_str());
    else if (type->p_count > 0)
    {
        string str = "Expression of type \'" + type->toString() + "\' is not numeric";
        ReportError(operation->lexeme, str.c_str());
    }
}

ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
{
    check_unary_plus_minus(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *type = operation->children.getFirst()->getType();
    if (*type == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryPlusInt;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryPlusDouble;
    return type;
}

ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
{
    check_unary_plus_minus(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *type = operation->children.getFirst()->getType();
    if (*type == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryMinusInt;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryMinusDouble;
    return type;
}

ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    TypeCastManager::Cast(operand, NPS_BOOL, false);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::NotBool;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseComplement(TBranch *operation) 
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_DOUBLE || operand_type->p_count > 0) {
        ReportError(operation->lexeme, incompatibleTypesError(operand->getType(),
                                                              operation->lexeme->lexeme).c_str());
        return nullptr;
    }
    if (*operand_type == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseNotInt;
    if (*operand_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseNotBool;
    return operand_type;
}

void check_binary_plus_minus(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL || // bool + *, * + bool
        (operand1_type->p_count > 0 && operand2_type->p_count > 0) || // type* + type*
        (*operand1_type == *NPS_DOUBLE && operand2_type->p_count > 0) || // double + type*
        (*operand2_type == *NPS_DOUBLE && operand1_type->p_count > 0)) // type* + double
    {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type,
                                                              operation->lexeme->lexeme).c_str());
        return;
    }
    if (*operand1_type == *NPS_DOUBLE)
        TypeCastManager::Cast(operation->children.getLast(), NPS_DOUBLE, false);
    if (*operand2_type == *NPS_DOUBLE)
        TypeCastManager::Cast(operation->children.getFirst(), NPS_DOUBLE, false);
}


ResultType* PrimitiveOperationsManager::nps_bPlus(TBranch *operation)
{
    check_binary_plus_minus(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (operand1_type->p_count + operand2_type->p_count == 0)
    {
        if (*operand1_type == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusDoubles;
        else
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusInts;
        return operand1_type;
    }
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusIntPointer;
    if (operand1_type->p_count > 0)
        return operand1_type;
    operation->children.add(operation->children.takeFirst());
    return operand2_type;
}

ResultType* PrimitiveOperationsManager::nps_bMinus(TBranch *operation)
{
    check_binary_plus_minus(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (operand1_type->p_count + operand2_type->p_count == 0)
    {
        if (*operand1_type == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryMinusDoubles;
        else
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryMinusInts;
        return operand1_type;
    }
    if (operand2_type->p_count > 0)
    {
        ReportError(operation->lexeme, "Can not subtract a pointer");
        return nullptr;
    }
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryMinusIntPointer;
    return operand1_type;
    
}

void check_mul_div(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
        operand1_type->p_count > 0 || operand2_type->p_count > 0)
    {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type,
                                                              operation->lexeme->lexeme).c_str());
        return;
    }
    if (*operand1_type == *NPS_DOUBLE)
        TypeCastManager::Cast(operation->children.getLast(), NPS_DOUBLE, false);
    if (*operand2_type == *NPS_DOUBLE)
        TypeCastManager::Cast(operation->children.getFirst(), NPS_DOUBLE, false);
}

ResultType* PrimitiveOperationsManager::nps_multiplication(TBranch *operation)
{
    check_mul_div(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (*operands_type == *NPS_DOUBLE)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::MultiplyDoubles;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::MultiplyInts;
    return operands_type;
}

ResultType* PrimitiveOperationsManager::nps_division(TBranch *operation)
{
    check_mul_div(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (*operands_type == *NPS_DOUBLE)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::DivideDoubles;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::DivideInts;
    return operands_type;
}

void check_both_ints(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type != *NPS_INT || *operand2_type != *NPS_INT)
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type,
                                                              operation->lexeme->lexeme).c_str());
}

ResultType* PrimitiveOperationsManager::nps_MOD(TBranch *operation)
{
    check_both_ints(operation);
    if (ErrorReported())
        return nullptr;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ModInts;
    return NPS_INT;
}

ResultType* PrimitiveOperationsManager::nps_leftShift(TBranch *operation)
{
    check_both_ints(operation);
    if (ErrorReported())
        return nullptr;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ShiftLeftInts;
    return NPS_INT;
}

ResultType* PrimitiveOperationsManager::nps_rightShift(TBranch *operation)
{
    check_both_ints(operation);
    if (ErrorReported())
        return nullptr;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ShiftRightInts;
    return NPS_INT;
}


void castBothToType(TBranch *operation, ResultType *type)
{
    TypeCastManager::Cast(operation->children.getFirst(), type, false);
    TypeCastManager::Cast(operation->children.getLast(), type, false);
}

void check_compare_availability(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
        operand1_type->p_count != operand2_type->p_count ||
        (*operand1_type != *operand2_type && operand1_type->p_count > 0))
    {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type,
                                                              operation->lexeme->lexeme).c_str());
        return;
    }
    if (operand1_type->p_count == 0)
        castBothToType(operation, NPS_DOUBLE);
}

ResultType* PrimitiveOperationsManager::nps_less(TBranch *operation)
{
    check_compare_availability(operation);
    if (ErrorReported())
        return nullptr;
    if (operation->children.getFirst()->getType()->p_count == 0)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessDoubles;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessPointers;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_lessEqual(TBranch *operation)
{
    check_compare_availability(operation);
    if (ErrorReported())
        return nullptr;
    if (operation->children.getFirst()->getType()->p_count == 0)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessEqualDoubles;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessEqualPointers;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_great(TBranch *operation)
{
    check_compare_availability(operation);
    if (ErrorReported())
        return nullptr;
    if (operation->children.getFirst()->getType()->p_count == 0)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMoreDoubles;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMorePointers;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_greatEqual(TBranch *operation)
{
    check_compare_availability(operation);
    if (ErrorReported())
        return nullptr;
    if (operation->children.getFirst()->getType()->p_count == 0)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMoreEqualDoubles;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMoreEqualPointers;
    return NPS_BOOL;
}

void check_equality_availability(TBranch *operation)
{
    TNode *operand1 = operation->children.getFirst();
    TNode *operand2 = operation->children.getLast();
    ResultType *operand1_type = operand1->getType();
    ResultType *operand2_type = operand2->getType();
    if (*operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0)
    {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type,
                                                              operation->lexeme->lexeme).c_str());
        return;
    }
    if (operand1_type->p_count > 0)
        return;
    if (*operand1_type == *NPS_BOOL)
        TypeCastManager::Cast(operand2, NPS_BOOL, false);
    else if (*operand2_type == *NPS_BOOL)
        TypeCastManager::Cast(operand1, NPS_BOOL, false);
    else
        castBothToType(operation, NPS_DOUBLE);
}

ResultType* PrimitiveOperationsManager::nps_equal(TBranch *operation)
{
    check_equality_availability(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (operands_type->p_count > 0)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualPointers;
    else if (*operands_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualDoubles;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_notEqual(TBranch *operation)
{
    check_equality_availability(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (operands_type->p_count > 0)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualPointers;
    else if (*operands_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualDoubles;
    return NPS_BOOL;
}

void check_bitwise_availability(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE ||
        operand1_type->p_count + operand2_type->p_count > 0 || *operand1_type != *operand2_type)
            ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type,
                                                              operation->lexeme->lexeme).c_str());
}

ResultType* PrimitiveOperationsManager::nps_bitwiseOR(TBranch *operation)
{
    check_bitwise_availability(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (*operands_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseOrBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseOrInts;
    return operands_type;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseXOR(TBranch *operation)
{
    check_bitwise_availability(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (*operands_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseXorBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseXorInts;
    return operands_type;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseAND(TBranch *operation)
{
    check_bitwise_availability(operation);
    if (ErrorReported())
        return nullptr;
    ResultType *operands_type = operation->children.getFirst()->getType();
    if (*operands_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseAndBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseAndInts;
    return operands_type;
}

ResultType* PrimitiveOperationsManager::nps_logicOR(TBranch *operation)
{
    castBothToType(operation, NPS_BOOL);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::LogicOrBools;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_logicAND(TBranch *operation)
{
    castBothToType(operation, NPS_BOOL);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::LogicAndBools;
    return NPS_BOOL;
}
