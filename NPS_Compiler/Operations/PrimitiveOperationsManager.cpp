//
// Created by Alexander on 01-Apr-17.
//

#include "PrimitiveOperationsManager.h"
#include "../ErrorReporter/ErrorReporter.h"
#include <cstring>
#include "../Types/TypesManager.h"
#include "../TypeCast/TypeCastManager.h"

char errorMessage[255];
ResultType *NPS_BOOL = TypesManager::GetResultType("bool");
ResultType *NPS_CHAR = TypesManager::GetResultType("char");
ResultType *NPS_DOUBLE = TypesManager::GetResultType("double");
ResultType *NPS_INT = TypesManager::GetResultType("int");

void castCharToInt(TBranch *operation)
{
    TypeCastManager::Cast(operation->children.getFirst(), NPS_INT, false);
}

void castCharToOperandType(TBranch *operation)
{
    TNode *op1 = operation->children.getFirst();
    TNode *op2 = operation->children.getLast();
    ResultType *op1type = op1->getType();
    ResultType *op2type = op2->getType();
    if (*op1type == *NPS_CHAR && op2type->p_count > 0
        || *op2type == *NPS_CHAR && op1type->p_count > 0)
        if (*op1type == *NPS_CHAR){
            TypeCastManager::Cast(op1, NPS_INT, false);
            return;
        }
        else {
            TypeCastManager::Cast(op2, NPS_INT, false);
            return;
        }
    if (*op1type == *NPS_CHAR && *op2type == *NPS_CHAR)    {
        TypeCastManager::Cast(op1, NPS_INT, false);
        TypeCastManager::Cast(op2, NPS_INT, false);
        return;
    }
    if (*op1type == *NPS_CHAR) {
        TypeCastManager::Cast(op1, op2type, false);
        return;
    }
    else {
        TypeCastManager::Cast(op2, op1type, false);
        return;
    }
}

ResultType *PrimitiveOperationsManager::GetResultOfOperation(TBranch *operation)
{
    switch(operation->children.count())
    {
        case 1:
            castCharToInt(operation);
            break;
        case 2:
            castCharToOperandType(operation);
            break;
        default:
            break;
    }
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
                return nps_dereference(operation);
            else // multiplication
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
        case 234: // & - ref
            if(operation->children.count() == 1)
                return nps_reference(operation);
            else // &
                return nps_bitwiseAND(operation);
        case 235: // &&
            return  nps_logicAND(operation);
        case 236: // |
            return nps_bitwiseOR(operation);
        case 237: // ^
            return nps_bitwiseXOR(operation);
        case 238: // ||
            return nps_logicOR(operation);
        case 240: // ? :
            return nps_ternaryOperator(operation);
        case 242: // ,
            return nps_comma(operation);
        default:
            break;
    }
    return nullptr;
}

const char *notNumericError(ResultType *operand)
{
    strcpy(errorMessage, "Expression of typeOfType \'");
    std::string str = operand->toString();
    const char *c = str.c_str();
    strcat(errorMessage, c);
    if (operand->p_count > 0)
        for (int i = 0; i < operand->p_count; i++)
            strcat(errorMessage, " *");
    strcat(errorMessage, "\' is not numeric");
    return errorMessage;
}

const char *incompatibleTypesError(ResultType *operand1, ResultType *operand2, const char *op)
{
    strcpy(errorMessage, "Binary operator \'");
    strcat(errorMessage, op);
    strcat(errorMessage, "\' can't be applied to the expressions of typeOfType \'");
    std::string str = operand1->toString();
    const char *c = str.c_str();
    strcat(errorMessage, c);
    for (int i = 0; i < operand1->p_count; i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\'");
    strcat(errorMessage, " and \'");
    str = operand2->toString();
    c = str.c_str();
    strcat(errorMessage, c);
    for (int i = 0; i < operand2->p_count; i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\'");
    return errorMessage;
}

const char *incompatibleTypesError(ResultType *operand, const char *op)
{
    strcpy(errorMessage, "Unary operator \'");
    strcat(errorMessage, op);
    strcat(errorMessage, "\' can't be applied to the expression of typeOfType \'");
    std::string str = operand->toString();
    const char *c = str.c_str();
    strcat(errorMessage, c);
    for (int i = 0; i < operand->p_count; i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\"");
    return errorMessage;
}

ResultType* PrimitiveOperationsManager::nps_increment(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_BOOL) {
        ReportError(operation->lexeme, incompatibleTypesError(operand_type, "++"));
        return 0;
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
        ReportError(operation->lexeme, incompatibleTypesError(operand_type, "--"));
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

ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_BOOL) {
        ReportError(operation->lexeme, incompatibleTypesError(operand_type, "+"));
        return 0;
    }
    if (operand_type->p_count > 0) {
        ReportError(operation->lexeme, notNumericError(operand_type));
        return 0;
    }
    if (*operand_type == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryPlusInt;
    if (*operand_type == *NPS_DOUBLE)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryPlusDouble;
    return operand_type;
}

ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_BOOL) {
        ReportError(operation->lexeme, incompatibleTypesError(operand_type, "-"));
        return 0;
    }
    if (operand_type->p_count > 0) {
        ReportError(operation->lexeme, notNumericError(operand_type));
        return 0;
    }
    if (*operand_type == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryMinusInt;
    if (*operand_type == *NPS_DOUBLE)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryMinusDouble;
    return operand_type;
}

ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type != *NPS_BOOL)
        TypeCastManager::Cast(operand, NPS_BOOL, false);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::NotBool;
    return operand_type;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseComplement(TBranch *operation) 
{
    TNode *operand = operation->children.getFirst();
    ResultType *operand_type = operand->getType();
    if (*operand_type == *NPS_DOUBLE || operand_type->p_count > 0) {
        ReportError(operation->lexeme, incompatibleTypesError(operand->getType(), "~"));
        return 0;
    }
    if (*operand_type == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseNotInt;
    if (*operand_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseNotBool;
    return operand_type;
}

ResultType* PrimitiveOperationsManager::nps_dereference(TBranch *operation)
{
    ResultType *operand_type = operation->children.getFirst()->getType();
    ResultType *result_operand = operand_type->clone();
    if (result_operand->p_count == 0) {
        ReportError(operation->lexeme, "Pointer type is required");
        return 0;
    }
    result_operand->p_count--;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Dereference;
    return result_operand;
}

ResultType* PrimitiveOperationsManager::nps_reference(TBranch *operation)
{
    ResultType *operand_type = operation->children.getFirst()->getType();
    ResultType *result_operand = operand_type->clone();
    if (result_operand->p_count < 0) {
        ReportError(operation->lexeme, "Address expression must me lvalue");
        return 0;
    }
    result_operand->p_count++;
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Reference;
    return result_operand;
}

ResultType *castIntToDouble(TBranch *operation)
{
    TNode *op1 = operation->children.getFirst();
    TNode *op2 = operation->children.getLast();
    ResultType *op1_type = op1->getType();
    ResultType *op2_type = op2->getType();
    if (*op1_type == *op2_type)
    {
        TypeCastManager::Cast(op1, NPS_DOUBLE, false);
        TypeCastManager::Cast(op2, NPS_DOUBLE, false);
    }
    if (*op1_type == *NPS_INT)
        TypeCastManager::Cast(op1, op2_type, false);
    else
        TypeCastManager::Cast(op2, op1_type, false);

    return NPS_DOUBLE;
}

ResultType* PrimitiveOperationsManager::nps_bPlus(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||               // bool + bool
            (operand1_type->p_count > 0 && operand2_type->p_count > 0) ||           // type* + type*
            (*operand1_type == *NPS_DOUBLE && operand2_type->p_count > 0) ||        // double + type*
            (*operand2_type == *NPS_DOUBLE && operand1_type->p_count > 0)){         // type* + double
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "+"));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE) {
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusDoubles;
            if (operand1_type != operand2_type)
                return castIntToDouble(operation);
        } else
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusInts;
        return operand1_type;
    } else {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryPlusIntPointer;
        if (*operand1_type == *NPS_INT)
            return operand2_type;
        else
            return operand1_type;
    }
}

ResultType* PrimitiveOperationsManager::nps_bMinus(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||           // bool - bool
        (operand1_type->p_count > 0 && operand2_type->p_count > 0) ||           // type* - type*
        (*operand1_type == *NPS_DOUBLE && operand2_type->p_count > 0) ||        // double - type*
        (*operand2_type == *NPS_DOUBLE && operand1_type->p_count > 0) ||        // type* - double
        operand2_type->p_count > 0) {                                           // type - type*
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "-"));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE) {
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryMinusDoubles;
            if (operand1_type != operand2_type)
                return castIntToDouble(operation);
        } else
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryMinusInts;
        return operand1_type;
    } else {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BinaryMinusIntPointer;
        if (*operand1_type == *NPS_INT)
            return operand2_type;
        else
            return operand1_type;
    }
}

ResultType* PrimitiveOperationsManager::nps_multiplication(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
            operand1_type->p_count > 0 || operand2_type->p_count > 0) {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "*"));
        return 0;
    }
    if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::MultiplyDoubles;
        if (operand1_type != operand2_type)
            return castIntToDouble(operation);
    } else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::MultiplyInts;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_division(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
        operand1_type->p_count > 0 || operand2_type->p_count > 0) {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "/"));
        return 0;
    }
    if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::DivideDoubles;
        if (operand1_type != operand2_type)
            return castIntToDouble(operation);
    } else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::DivideInts;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_MOD(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_INT && *operand2_type == *NPS_INT){
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ModInts;
        return operand1_type;
    }
    else {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "%"));
        return 0;
    }
}

ResultType* PrimitiveOperationsManager::nps_leftShift(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_INT && *operand2_type == *NPS_INT){
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ShiftLeftInts;
        return operand1_type;
    }
    else {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "<<"));
        return 0;
    }
}

ResultType* PrimitiveOperationsManager::nps_rightShift(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_INT && *operand2_type == *NPS_INT){
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ShiftRigthInts;
        return operand1_type;
    }
    else {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, ">>"));
        return 0;
    }
}

ResultType* PrimitiveOperationsManager::nps_less(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
            operand1_type->p_count != operand2_type->p_count ||
            *operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0){
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "<"));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessDoubles;
        if (*operand1_type == *NPS_INT || *operand2_type == *NPS_INT)
            return castIntToDouble(operation);
    } else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessPointers;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_lessEqual(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
        operand1_type->p_count != operand2_type->p_count ||
        *operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0){
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "<="));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessEqualDoubles;
        if (*operand1_type == *NPS_INT || *operand2_type == *NPS_INT)
            return castIntToDouble(operation);
    } else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpLessEqualPointers;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_great(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
        operand1_type->p_count != operand2_type->p_count ||
        *operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0){
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, ">"));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMoreDoubles;
        if (*operand1_type == *NPS_INT || *operand2_type == *NPS_INT)
            return castIntToDouble(operation);
    } else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMorePointers;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_greatEqual(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_BOOL || *operand2_type == *NPS_BOOL ||
        operand1_type->p_count != operand2_type->p_count ||
        *operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0){
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, ">="));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMoreEqualDoubles;
        if (*operand1_type == *NPS_INT || *operand2_type == *NPS_INT)
            return castIntToDouble(operation);
    } else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpMoreEqualPointers;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_equal(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0 ||
            *operand1_type == *NPS_BOOL && *operand2_type != *NPS_BOOL ||
            *operand1_type != *NPS_BOOL && *operand2_type == *NPS_BOOL){
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "=="));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE) {
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualDoubles;
            if (operand1_type != operand2_type)
                return castIntToDouble(operation);
        } else
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualBools;
        return operand1_type;
    } else {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpEqualPointers;
        return operand1_type;
    }
}

ResultType* PrimitiveOperationsManager::nps_notEqual(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type != *operand2_type && operand1_type->p_count + operand2_type->p_count > 0 ||
        *operand1_type == *NPS_BOOL && *operand2_type != *NPS_BOOL ||
        *operand1_type != *NPS_BOOL && *operand2_type == *NPS_BOOL){
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "!="));
        return 0;
    }
    if (operand1_type->p_count + operand2_type->p_count == 0) {
        if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE) {
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualDoubles;
            if (operand1_type != operand2_type)
                return castIntToDouble(operation);
        } else
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualBools;
        return operand1_type;
    } else {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::CmpNotEqualPointers;
        return operand1_type;
    }
}

ResultType* PrimitiveOperationsManager::nps_bitwiseOR(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE ||
        operand1_type->p_count + operand2_type->p_count > 0 ||
        *operand1_type != *operand2_type) {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "|"));
        return 0;
    }
    if (*operand1_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseOrBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseOrInts;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseXOR(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE ||
        operand1_type->p_count > 0 || operand2_type->p_count > 0 ||
        *operand1_type != *operand2_type) {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "^"));
        return 0;
    }
    if (*operand1_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseXorBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseXorInts;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseAND(TBranch *operation)
{
    ResultType *operand1_type = operation->children.getFirst()->getType();
    ResultType *operand2_type = operation->children.getLast()->getType();
    if (*operand1_type == *NPS_DOUBLE || *operand2_type == *NPS_DOUBLE ||
        operand1_type->p_count > 0 || operand2_type->p_count > 0 ||
        *operand1_type != *operand2_type) {
        ReportError(operation->lexeme, incompatibleTypesError(operand1_type, operand2_type, "&"));
        return 0;
    }
    if (*operand1_type == *NPS_BOOL)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseAndBools;
    else
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::BitwiseAndInts;
    return operand1_type;
}

ResultType* PrimitiveOperationsManager::nps_logicOR(TBranch *operation)
{
    TNode *operand1 = operation->children.getFirst();
    TNode *operand2 = operation->children.getLast();
    ResultType *operand1_type = operand1->getType();
    ResultType *operand2_type = operand2->getType();
    if (*operand1_type != *NPS_BOOL)
        TypeCastManager::Cast(operand1, NPS_BOOL, false);
    if (*operand2_type != *NPS_BOOL)
        TypeCastManager::Cast(operand2, NPS_BOOL, false);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::LogicOrBools;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_logicAND(TBranch *operation)
{
    TNode *operand1 = operation->children.getFirst();
    TNode *operand2 = operation->children.getLast();
    ResultType *operand1_type = operand1->getType();
    ResultType *operand2_type = operand2->getType();
    if (*operand1_type != *NPS_BOOL)
        TypeCastManager::Cast(operand1, NPS_BOOL, false);
    if (*operand2_type != *NPS_BOOL)
        TypeCastManager::Cast(operand2, NPS_BOOL, false);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::LogicAndBools;
    return NPS_BOOL;
}

ResultType* PrimitiveOperationsManager::nps_comma(TBranch *operation)
{
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Comma;
    return operation->children.getLast()->getType()->clone();
}

ResultType* PrimitiveOperationsManager::nps_ternaryOperator(TBranch *operation)
{
    ResultType *condition = operation->children.get(0)->getType();
    ResultType *operand1_type = operation->children.get(1)->getType();
    ResultType *operand2_type = operation->children.get(2)->getType();
    if (*condition != *NPS_BOOL) {
        ReportError(operation->lexeme, "Not bool in condition");
        return 0;
    }
    if (*operand1_type == *operand2_type) {
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Ternary;
        return operand1_type;
    }
    else {
        strcpy(errorMessage, "Types \'");
        std::string str = operand1_type->toString();
        const char *c = str.c_str();
        strcat(errorMessage, c);
        for (int i = 0; i < operand1_type->p_count; i++)
            strcat(errorMessage, " *");
        strcat(errorMessage, "\'");
        strcat(errorMessage, " and \'");
        str = operand1_type->toString();
        c = str.c_str();
        strcat(errorMessage, c);
        for (int i = 0; i < operand2_type->p_count; i++)
            strcat(errorMessage, " *");
        strcat(errorMessage, "\' are not compatible");
        ReportError(operation->lexeme, errorMessage);
        return 0;
    }
}

//
//ResultType* PrimitiveOperationsManager::nps_increment(TBranch *operation) {
//    ResultType *operand = operation->children.getFirst()->getType();
//    if (*operand == NPS_BOOL || operand->isConst)
//        if (*operand == NPS_BOOL) {
//            ReportError(operation->lexeme, incompatibleTypesError(operand, "++"));
//            return 0;
//        }
//        else
//        {
//            ReportError(operation->lexeme, notAssignableError());
//            return 0;
//        }
//    return new ResultType(operand->baseType,
//                          operand->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_decrement(TBranch *operation)
//{
//    ResultType *operand = operation->children.getFirst()->getType();
//    if (*operand == NPS_BOOL || operand->isConst)
//        if (*operand == NPS_BOOL) {
//            ReportError(operation->lexeme, incompatibleTypesError(operand, "--"));
//            return 0;
//        }
//        else
//        {
//            ReportError(operation->lexeme, notAssignableError());
//            return 0;
//        }
//    return new ResultType(operand->baseType,
//                          operand->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
//{
//    ResultType *operand = operation->children.getFirst()->getType();
//    return new ResultType("bool",
//                          operand->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_bitwiseComplement(TBranch *operation)
//{
//    ResultType *operand = operation->children.getFirst()->getType();
//    if(*operand == NPS_DOUBLE || operand->p_count > 0){
//        ReportError(operation->lexeme, incompatibleTypesError(operand, "~"));
//        return 0;
//    }
//    return new ResultType("int",
//                          operand->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
//{
//    ResultType *operand = operation->children.getFirst()->getType();
//    if (*operand == NPS_BOOL || operand->p_count > 0)
//        if (operand->p_count > 0) {
//            ReportError(operation->lexeme, notNumericError(operand));
//            return 0;
//        }
//        else
//        {
//            ReportError(operation->lexeme, incompatibleTypesError(operand, "+"));
//            return 0;
//        }
//    if (*operand == NPS_DOUBLE)
//        return new ResultType("double",
//                              operand->p_count,
//                              true);
//    else
//        return new ResultType("int",
//                              operand->p_count,
//                              true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
//{
//    ResultType *operand = operation->children.getFirst()->getType();
//    if (*operand == NPS_BOOL || operand->p_count > 0)
//        if (operand->p_count > 0) {
//            ReportError(operation->lexeme, notNumericError(operand));
//            return 0;
//        }
//        else
//        {
//            ReportError(operation->lexeme, incompatibleTypesError(operand, "-"));
//            return 0;
//        }
//    if (*operand == NPS_DOUBLE)
//        return new ResultType("double",
//                              operand->p_count,
//                              true);
//    else
//        return new ResultType("int",
//                              operand->p_count,
//                              true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_bPlus(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
//        if (*operand1 == NPS_BOOL)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "+"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "+"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0) {
//        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//            return new ResultType("double",
//                                  operand1->p_count,
//                                  true);
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    }
//    if (operand1->p_count > 0 || operand2->p_count > 0) {
//        if (operand1->p_count > 0 && operand2->p_count > 0) {
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "+"));
//            return 0;
//        }
//        if (*operand1 == NPS_DOUBLE && operand1->p_count == 0 ||
//            *operand2 == NPS_DOUBLE && operand2->p_count == 0) {
//            if (*operand1 == NPS_DOUBLE)
//                ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "+"));
//            else
//                ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "+"));
//            return 0;
//        }
//        if (operand1->p_count > 0)
//            return new ResultType("int",
//                                  operand1->p_count,
//                                  true);
//        if (operand2->p_count > 0)
//            return new ResultType("int",
//                                  operand2->p_count,
//                                  true);
//    }
//    strcpy(errorMessage, "Unknown error");
//    ReportError(operation->lexeme, errorMessage);
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_bMinus(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
//        if (*operand1 == NPS_BOOL)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "-"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "-"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0) {
//        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//            return new ResultType("double",
//                                  operand1->p_count,
//                                  true);
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    }
//    if (operand2->p_count > 0){
//        if (*operand1 == NPS_BOOL)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "-"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "-"));
//        return 0;
//    }
//    if (operand1->p_count > 0) {
//        if (*operand2 == NPS_DOUBLE) {
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "-"));
//            return 0;
//        }
//        if (operand1 == operand2)
//            return new ResultType("int",
//                                  operand1->p_count,
//                                  true);
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    }
//    strcpy(errorMessage, "Unknown error");
//    ReportError(operation->lexeme, errorMessage);
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_multiplication(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
//        if (*operand1 == NPS_BOOL)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "*"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "*"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0) {
//        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//            return new ResultType("double",
//                                  operand1->p_count,
//                                  true);
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    }
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "*"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_dereference(TBranch *operation)
//{
//    if (operation->children.getFirst()->getType()->p_count == 0) {
//        strcpy(errorMessage, "Pointer typeOfType is required");
//        ReportError(operation->children.getFirst()->lexeme, errorMessage);
//        return 0;
//    }
//    return new ResultType("int",
//                          operation->children.getFirst()->getType()->p_count - 1,
//                          operation->children.getFirst()->getType()->isConst);
//}
//
//ResultType* PrimitiveOperationsManager::nps_reference(TBranch *operation)
//{
//    return new ResultType("int",
//                          operation->children.getFirst()->getType()->p_count + 1,
//                          operation->children.getFirst()->getType()->isConst);
//}
//
//ResultType* PrimitiveOperationsManager::nps_division(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
//        if (*operand1 == NPS_BOOL)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "/"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "/"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0) {
//        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//            return new ResultType("double",
//                                  operand1->p_count,
//                                  true);
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    }
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "/"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_MOD(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL ||
//        *operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//    {
//        if (*operand1 == NPS_BOOL || *operand1 == NPS_DOUBLE)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "%"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "%"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "%"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_leftShift(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL ||
//        *operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//    {
//        if (*operand1 == NPS_BOOL || *operand1 == NPS_DOUBLE)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "<<"));
//        else
//            ReportError(operation->lexeme, errorMessage);
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "<<"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_rightShift(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL ||
//        *operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//    {
//        if (*operand1 == NPS_BOOL || *operand1 == NPS_DOUBLE)
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, ">>"));
//        else
//            ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, ">>"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, ">>"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_less(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (operand1 != operand2)
//    {
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "<"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("bool",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "<"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_great(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (operand1 != operand2){
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, ">"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("bool",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, ">"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_relationEqual(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (operand1 != operand2){
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "=="));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("bool",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "=="));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_notEqual(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (operand1 != operand2){
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "!="));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("bool",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "!="));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_bitwiseAND(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE){
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "&"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "&"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_bitwiseXOR(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
//    {
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "^"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "^"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_bitwiseOR(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE){
//        ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "|"));
//        return 0;
//    }
//    if (operand1->p_count + operand2->p_count == 0)
//        return new ResultType("int",
//                              operand1->p_count,
//                              true);
//    ReportError(operation->lexeme, incompatibleTypesError(operand1, operand2, "|"));
//    return 0;
//}
//
//ResultType* PrimitiveOperationsManager::nps_logicAND(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    return new ResultType("bool",
//                          operand1->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_logicOR(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    return new ResultType("bool",
//                          operand1->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_ternaryOperator(TBranch *operation)
//{
//    ResultType *condition = operation->children.get(0)->getType();
//    ResultType *operand1 = operation->children.get(1)->getType();
//    ResultType *operand2 = operation->children.get(2)->getType();
//    if (*condition != NPS_BOOL)
//        return nullptr;
//    if (operand1 == operand2)
//        return new ResultType(operand1->baseType,
//                              operand1->p_count,
//                              operand1->isConst);
//    else {
//        strcpy(errorMessage, "Types \'");
//        strcat(errorMessage, operand1->baseType);
//        for (int i = 0; i < operand1->p_count; i++)
//            strcat(errorMessage, " *");
//        strcat(errorMessage, "\'");
//        strcat(errorMessage, " and \'");
//        strcat(errorMessage, operand2->baseType);
//        for (int i = 0; i < operand2->p_count; i++)
//            strcat(errorMessage, " *");
//        strcat(errorMessage, "\' are not compatible");
//        ReportError(operation->lexeme, errorMessage);
//        return 0;
//    }
//}
//
//ResultType* PrimitiveOperationsManager::nps_comma(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    return new ResultType(operand2->baseType,
//                          operand2->p_count,
//                          true);
//}
//
//ResultType* PrimitiveOperationsManager::nps_equal(TBranch *operation)
//{
//    ResultType *operand1 = operation->children.getFirst()->getType();
//    ResultType *operand2 = operation->children.getLast()->getType();
//    if (operand1->isConst)
//    {
//        ReportError(operation->lexeme, notAssignableError());
//        return 0;
//    }
//    if (operand1 == operand2)
//        return new ResultType(operand1->baseType,
//                              operand1->p_count,
//                              operand1->isConst);
//    strcpy(errorMessage, "Unknown error");
//    ReportError(operation->lexeme, errorMessage);
//    return 0;
//}