//
// Created by Alexander on 01-Apr-17.
//

#include <iostream>
#include "PrimitiveOperationsManager.h"
#include "../ErrorReporter/ErrorReporter.h"
#include <cstring>

const ResultType PrimitiveOperationsManager::NPS_BOOL = ResultType("bool");
const ResultType PrimitiveOperationsManager::NPS_CHAR = ResultType("char");
const ResultType PrimitiveOperationsManager::NPS_DOUBLE = ResultType("double");
const ResultType PrimitiveOperationsManager::NPS_INT = ResultType("int");
char errorMessage[255];

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
        case 239: // ? :
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
    std::cout << errorMessage << std::endl;
    return errorMessage;
}

const char *notNumericError(ResultType *operand)
{
    strcpy(errorMessage, "Expression of type \'");
    strcat(errorMessage, operand->GetBaseType());
    if (operand->GetPCount() > 0)
        for (int i = 0; i < operand->GetPCount(); i++)
            strcat(errorMessage, " *");
    strcat(errorMessage, "\' is not numeric");
    std::cout << errorMessage << std::endl;
    return errorMessage;
}

const char *incompatibleTypesError(ResultType *operand1, ResultType *operand2, const char *op)
{
    strcpy(errorMessage, "Binary operator \'");
    strcat(errorMessage, op);
    strcat(errorMessage, "\' can't be applied to the expressions of type \'");
    strcat(errorMessage, operand1->GetBaseType());
    for (int i = 0; i < operand1->GetPCount(); i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\'");
    strcat(errorMessage, " and \'");
    strcat(errorMessage, operand2->GetBaseType());
    for (int i = 0; i < operand2->GetPCount(); i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\'");
    std::cout << errorMessage << std::endl;
    return errorMessage;
}

const char *incompatibleTypesError(ResultType *operand, const char *op)
{
    strcpy(errorMessage, "Unary operator \'");
    strcat(errorMessage, op);
    strcat(errorMessage, "\' can't be applied to the expression of type \'");
    strcat(errorMessage, operand->GetBaseType());
    for (int i = 0; i < operand->GetPCount(); i++)
        strcat(errorMessage, " *");
    strcat(errorMessage, "\"");
    std::cout << errorMessage << std::endl;
    return errorMessage;
}

ResultType* PrimitiveOperationsManager::nps_increment(TBranch *operation) {
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == NPS_BOOL || operand->isConst())
        if (*operand == NPS_BOOL) {
            ReportError(operand->GetBaseType(), incompatibleTypesError(operand, "++"));
            return 0;
        }
        else
        {
            ReportError(operand->GetBaseType(), notAssignableError());
            return 0;
        }
    return new ResultType(operand->GetBaseType(),
                          operand->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_decrement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == NPS_BOOL || operand->isConst())
        if (*operand == NPS_BOOL) {
            ReportError(operand->GetBaseType(), incompatibleTypesError(operand, "--"));
            return 0;
        }
        else
        {
            ReportError(operand->GetBaseType(), notAssignableError());
            return 0;
        }
    return new ResultType(operand->GetBaseType(),
                          operand->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_logicComplement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    return new ResultType("bool",
                          operand->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_bitwiseComplement(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if(*operand == NPS_DOUBLE || operand->GetPCount() > 0){
        ReportError(operand->GetBaseType(), incompatibleTypesError(operand, "~"));
        return 0;
    }
    return new ResultType("int",
                          operand->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_uPlus(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == NPS_BOOL || operand->GetPCount() > 0)
        if (operand->GetPCount() > 0) {
            ReportError(operand->GetBaseType(), notNumericError(operand));
            return 0;
        }
        else
        {
            ReportError(operand->GetBaseType(), incompatibleTypesError(operand, "+"));
            return 0;
        }
    if (*operand == NPS_DOUBLE)
        return new ResultType("double",
                              operand->GetPCount(),
                              true);
    else
        return new ResultType("int",
                              operand->GetPCount(),
                              true);
}

ResultType* PrimitiveOperationsManager::nps_uMinus(TBranch *operation)
{
    ResultType *operand = operation->children.getFirst()->getType();
    if (*operand == NPS_BOOL || operand->GetPCount() > 0)
        if (operand->GetPCount() > 0) {
            ReportError(operand->GetBaseType(), notNumericError(operand));
            return 0;
        }
        else
        {
            ReportError(operand->GetBaseType(), incompatibleTypesError(operand, "-"));
            return 0;
        }
    if (*operand == NPS_DOUBLE)
        return new ResultType("double",
                              operand->GetPCount(),
                              true);
    else
        return new ResultType("int",
                              operand->GetPCount(),
                              true);
}

ResultType* PrimitiveOperationsManager::nps_bPlus(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
        if (*operand1 == NPS_BOOL)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "+"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "+"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0) {
        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
            return new ResultType("double",
                                  operand1->GetPCount(),
                                  true);
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    }
    if (operand1->GetPCount() > 0 || operand2->GetPCount() > 0) {
        if (operand1->GetPCount() > 0 && operand2->GetPCount() > 0) {
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "+"));
            return 0;
        }
        if (*operand1 == NPS_DOUBLE && operand1->GetPCount() == 0 ||
                *operand2 == NPS_DOUBLE && operand2->GetPCount() == 0) {
            if (*operand1 == NPS_DOUBLE)
                ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "+"));
            else
                ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "+"));
            return 0;
        }
        if (operand1->GetPCount() > 0)
            return new ResultType("int",
                                  operand1->GetPCount(),
                                  true);
        if (operand2->GetPCount() > 0)
            return new ResultType("int",
                                  operand2->GetPCount(),
                                  true);
    }
    strcpy(errorMessage, "Unknown error");
    std::cout << errorMessage << std::endl;
    ReportError(operand1->GetBaseType(), errorMessage);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bMinus(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
        if (*operand1 == NPS_BOOL)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "-"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "-"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0) {
        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
            return new ResultType("double",
                                  operand1->GetPCount(),
                                  true);
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    }
    if (operand2->GetPCount() > 0){
        if (*operand1 == NPS_BOOL)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "-"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "-"));
        return 0;
    }
    if (operand1->GetPCount() > 0) {
        if (*operand2 == NPS_DOUBLE) {
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "-"));
            return 0;
        }
        if (operand1 == operand2)
            return new ResultType("int",
                                  operand1->GetPCount(),
                                  true);
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    }
    strcpy(errorMessage, "Unknown error");
    std::cout << errorMessage << std::endl;
    ReportError(operand1->GetBaseType(), errorMessage);
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_multiplication(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
        if (*operand1 == NPS_BOOL)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "*"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "*"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0) {
        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
            return new ResultType("double",
                                  operand1->GetPCount(),
                                  true);
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    }
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "*"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_indirection(TBranch *operation)
{
    if (operation->children.getFirst()->getType()->GetPCount() == 0) {
        strcpy(errorMessage, "Pointer type is required");
        ReportError(operation->children.getFirst()->getType()->GetBaseType(), errorMessage);
        return 0;
    }
    return new ResultType("int",
                          operation->children.getFirst()->getType()->GetPCount() - 1,
                          operation->children.getFirst()->getType()->isConst());
}

ResultType* PrimitiveOperationsManager::nps_reference(TBranch *operation)
{
    return new ResultType("int",
                          operation->children.getFirst()->getType()->GetPCount() + 1,
                          operation->children.getFirst()->getType()->isConst());
}

ResultType* PrimitiveOperationsManager::nps_division(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL){
        if (*operand1 == NPS_BOOL)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "/"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "/"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0) {
        if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
            return new ResultType("double",
                                  operand1->GetPCount(),
                                  true);
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    }
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "/"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_MOD(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL ||
        *operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
    {
        if (*operand1 == NPS_BOOL || *operand1 == NPS_DOUBLE)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "%"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, "%"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "%"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_leftShift(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL ||
        *operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
    {
        if (*operand1 == NPS_BOOL || *operand1 == NPS_DOUBLE)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "<<"));
        else
            ReportError(operand2->GetBaseType(), errorMessage);
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "<<"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_rightShift(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_BOOL || *operand2 == NPS_BOOL ||
        *operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
    {
        if (*operand1 == NPS_BOOL || *operand1 == NPS_DOUBLE)
            ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, ">>"));
        else
            ReportError(operand2->GetBaseType(), incompatibleTypesError(operand1, operand2, ">>"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, ">>"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_less(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (operand1 != operand2)
    {
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "<"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("bool",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "<"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_great(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (operand1 != operand2){
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, ">"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("bool",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, ">"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_relationEqual(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (operand1 != operand2){
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "=="));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("bool",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "=="));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_notEqual(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (operand1 != operand2){
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "!="));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("bool",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "!="));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseAND(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE){
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "&"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "&"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseXOR(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE)
    {
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "^"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "^"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_bitwiseOR(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (*operand1 == NPS_DOUBLE || *operand2 == NPS_DOUBLE){
        ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "|"));
        return 0;
    }
    if (operand1->GetPCount() + operand2->GetPCount() == 0)
        return new ResultType("int",
                              operand1->GetPCount(),
                              true);
    ReportError(operand1->GetBaseType(), incompatibleTypesError(operand1, operand2, "|"));
    return 0;
}

ResultType* PrimitiveOperationsManager::nps_logicAND(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    return new ResultType("bool",
                          operand1->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_logicOR(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    return new ResultType("bool",
                          operand1->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_ternaryOperator(TBranch *operation)
{
    ResultType *condition = operation->children.get(0)->getType();
    ResultType *operand1 = operation->children.get(1)->getType();
    ResultType *operand2 = operation->children.get(2)->getType();
    if (operand1 == operand2)
        return new ResultType(operand1->GetBaseType(),
                              operand1->GetPCount(),
                              operand1->isConst());
    else {
        strcpy(errorMessage, "Types \'");
        strcat(errorMessage, operand1->GetBaseType());
        for (int i = 0; i < operand1->GetPCount(); i++)
            strcat(errorMessage, " *");
        strcat(errorMessage, "\'");
        strcat(errorMessage, " and \'");
        strcat(errorMessage, operand2->GetBaseType());
        for (int i = 0; i < operand2->GetPCount(); i++)
            strcat(errorMessage, " *");
        strcat(errorMessage, "\' are not compatible");
        ReportError(operand1->GetBaseType(), errorMessage);
        return 0;
    }
}

ResultType* PrimitiveOperationsManager::nps_comma(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    return new ResultType(operand2->GetBaseType(),
                          operand2->GetPCount(),
                          true);
}

ResultType* PrimitiveOperationsManager::nps_equal(TBranch *operation)
{
    ResultType *operand1 = operation->children.getFirst()->getType();
    ResultType *operand2 = operation->children.getLast()->getType();
    if (operand1->isConst())
    {
        ReportError(operand1->GetBaseType(), notAssignableError());
        return 0;
    }
    if (operand1 == operand2)
        return new ResultType(operand1->GetBaseType(),
                              operand1->GetPCount(),
                              operand1->isConst());
    strcpy(errorMessage, "Unknown error");
    ReportError(operand1->GetBaseType(), errorMessage);
    return 0;
}