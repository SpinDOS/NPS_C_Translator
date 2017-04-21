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
    TypeCastManager::TryCast(operation, NPS_INT);
}

void castCharToOperandType(TBranch *operation)
{
    TNode *op1 = operation->children.getFirst();
    TNode *op2 = operation->children.getLast();
    if (*op1->getType() == *NPS_CHAR && op2->getType()->p_count > 0
        || *op2->getType() == *NPS_CHAR && op1->getType()->p_count > 0)
        if (*op1->getType() == *NPS_CHAR){
            TypeCastManager::TryCast(op1, NPS_INT);
            return;
        }
        else {
            TypeCastManager::TryCast(op2, NPS_INT);
            return;
        }
    if (*op1->getType() == *NPS_CHAR && *op1->getType() == *NPS_CHAR)    {
        TypeCastManager::TryCast(op1, NPS_INT);
        TypeCastManager::TryCast(op2, NPS_INT);
        return;
    }
    if (*op1->getType() == *NPS_CHAR) {
        TypeCastManager::TryCast(op1, op2->getType());
        return;
    }
    else {
        TypeCastManager::TryCast(op2, op1->getType());
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
        case 3:
            //  TODO
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
                return nps_indirection(operation);
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
        case 235: // &&
            return  nps_logicAND(operation);
        case 236: // |
            return nps_bitwiseOR(operation);
        case 237: // ^
            return nps_bitwiseXOR(operation);
        case 238: // &&
            return nps_logicOR(operation);
        case 240: // ? :
            return nps_ternaryOperator(operation);
        case 241: // =
            return nps_equal(operation);
        case 242: // ,
            return nps_comma(operation);
        default:
            break;
    }
    return nullptr;
}

const char *notAssignableError()
{
    strcpy(errorMessage, "Expression is not assignable");
    return errorMessage;
}

const char *notNumericError(ResultType *operand)
{
    strcpy(errorMessage, "Expression of typeOfType \'");
    strcat(errorMessage, operand->baseType);
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
    strcat(errorMessage, operand1->baseType);
    for (int i = 0; i < operand1->p_count; i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\'");
    strcat(errorMessage, " and \'");
    strcat(errorMessage, operand2->baseType);
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
    strcat(errorMessage, operand->baseType);
    for (int i = 0; i < operand->p_count; i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\"");
    return errorMessage;
}

ResultType *NewResultType (BaseType *base_type, int p_count)
{
    ResultType *result = new ResultType;
    result->baseType = base_type;
    result->p_count = p_count;
    return result;
}

ResultType* PrimitiveOperationsManager::nps_increment(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == *NPS_BOOL)
        ReportError(operation->lexeme, incompatibleTypesError(operand, "++"));
    if (operation->IsLeftAssociated) // postfix
    {
        if (*operand == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixIncInt;
        if (*operand == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixIncDouble;
        if (operand->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixIncPointer;
        return operand->clone();
    }
    else // prefix
    {
        if (*operand == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixIncInt;
        if (*operand == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixIncDouble;
        if (operand->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixIncPointer;
        return operand->clone();
    }
}

ResultType* PrimitiveOperationsManager::nps_decrement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == *NPS_BOOL)
        ReportError(operation->lexeme, incompatibleTypesError(operand, "--"));
    if (operation->IsLeftAssociated) // postfix
    {
        if (*operand == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixDecInt;
        if (*operand == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixDecDouble;
        if (operand->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PostfixDecPointer;
        return operand->clone();
    }
    else // prefix
    {
        if (*operand == *NPS_INT)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixDecInt;
        if (*operand == *NPS_DOUBLE)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixDecDouble;
        if (operand->p_count > 0)
            operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::PrefixDecPointer;
        return operand->clone();
    }
}

ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == *NPS_BOOL)
        ReportError(operation->lexeme, incompatibleTypesError(operand, "+"));
    if (*operand == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryPlusInt;
    if (*operand == *NPS_DOUBLE)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryPlusDouble;
    return operand->clone();
}

ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == *NPS_BOOL)
        ReportError(operation->lexeme, incompatibleTypesError(operand, "-"));
    if (*operand == *NPS_INT)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryMinusInt;
    if (*operand == *NPS_DOUBLE)
        operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::UnaryMinusDouble;
    return operand->clone();
}

ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
{
    TNode *operand = operation->children.getFirst();
    if (*operand->getType() != *NPS_BOOL)
        TypeCastManager::TryCast(operand, NPS_BOOL);
    operation->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::NotBool;
    return operand->getType()->clone();
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
//ResultType* PrimitiveOperationsManager::nps_indirection(TBranch *operation)
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