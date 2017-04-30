//
// Created by Alexander on 30-Apr-17.
//

#include "TNode.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

using namespace NPS_Compiler;

bool IsLeftAssociated(int priority);

TLeaf* NPS_Compiler::GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(lexeme, "Operation expected");
        return nullptr;
    }
    hasLeft = true;
    expectedRight = false;
    if (400 <= lexeme->code && lexeme->code < 600) // variable name
        return new TVariable(lexeme);
    
    TConstant *result = new TConstant(lexeme);
    if (lexeme->code == 100) // string constant
    {
        result->constantType = TypesManager::GetResultType("char")->clone();
        result->constantType->p_count = 1;
        result->data = parse_string_constant(*lexeme);
        return result;
    }
    if (lexeme->code == 110) // char constant
    {
        result->constantType = TypesManager::GetResultType("char");
        char temp = parse_char_constant(*lexeme);
        result->data = Heap::get_mem(1);
        memcpy(result->data, &temp, 1);
        return result;
    }
    if (150 <= lexeme->code && lexeme->code < 160) // bool constant
    {
        result->constantType = TypesManager::GetResultType("bool");
        result->data = Heap::get_mem(1);
        bool temp = parse_bool_constant(*lexeme);
        memcpy(result->data, &temp, 1);
        return result;
    }
    // numeric constant
    char *type;
    double data = parse_num_constant(*lexeme, &type);
    result->data = Heap::get_mem(sizeof(double));
    memcpy(result->data, &data, sizeof(double));
    result->constantType = TypesManager::GetResultType(type);
    Heap::free_mem(type);
    return result;
}

TOperation* NPS_Compiler::GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    TOperation* result = new TOperation(lexeme);
    switch (lexeme->code)
    {
        case 214: // !
        case 215: // ~
            if (hasLeft) {
                ReportError(lexeme, "No expected left operand (~!)");
                return nullptr;
            }
            result->Priority = 23;
            result->NumOfChildren = 1;
            expectedRight = true;
            hasLeft = false;
            break;
        case 218: // *
            if(hasLeft)
            {
                result->Priority = 25;
                result->NumOfChildren = 2;
                expectedRight = true;
                hasLeft = false;
            }
            else
            {
                result->Priority = 23;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
            }
            break;
        case 219: // /
        case 220: // %
            if (!hasLeft) {
                ReportError(lexeme, "expected left operand (% /)");
                return nullptr;
            }
            result->Priority = 25;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 221: // + -
        case 222:
            if (hasLeft) {
                result->Priority = 26;
                result->NumOfChildren = 2;
                hasLeft = false;
                expectedRight = true;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
            }
            break;
        case 202: // ++ --
        case 203:
            if (hasLeft) {
                result->Priority = 22;
                result->NumOfChildren = 1;
                hasLeft = true;
                expectedRight = false;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                hasLeft = false;
                expectedRight = true;
            }
            break;
        case 241: // =
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand (=)");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 225: // <
        case 226: // <=
        case 227: // >
        case 228: // >=
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand ( < > <= >=");
                return nullptr;
            }
            result->Priority = 28;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 234: // &
            if (hasLeft) {
                result->Priority = 30;
                result->NumOfChildren = 2;
                hasLeft = false;
                expectedRight = true;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                hasLeft = false;
                expectedRight = true;
            }
            break;
        case 237: // ^
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand ^");
                return nullptr;
            }
            result->Priority = 31;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 236: // |
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand |");
                return nullptr;
            }
            result->Priority = 32;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 238: // ||
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand ||");
                return nullptr;
            }
            result->Priority = 34;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 242: // ,
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand ,");
                return nullptr;
            }
            result->Priority = 37;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 239: // ?
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand ?:");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 3;
            hasLeft = false;
            expectedRight = true;
            break;
        case 240:  // :
            if(expectedRight)
            {
                ReportError(lexeme, "Expected right operand");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 1;
            hasLeft = false;
            expectedRight = true;
            break;
        case 229: // ==
        case 233: // !=
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand != ==");
                return nullptr;
            }
            result->Priority = 29;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 208: // .
        case 209: // ->
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand as object ->");
                return nullptr;
            }
            result->Priority = 22;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 223: // <<
        case 224: // >>
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand << >>");
                return nullptr;
            }
            result->Priority = 27;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 235: // &&
            if (!hasLeft) {
                ReportError(lexeme, "Expected left operand &&");
                return nullptr;
            }
            result->Priority = 33;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 243: // ;
            if(expectedRight)
            {
                ReportError(lexeme, "Expected right operand");
                return nullptr;
            }
            result->Priority = 40;
            result->NumOfChildren = 0;
            hasLeft = false;
            expectedRight = false;
            break;
        case 206: // [
            if(!hasLeft)
            {
                ReportError(lexeme, "Expected left operand");
                return nullptr;
            }
            result->Priority = 20;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 207: // ]
            if(!hasLeft)
            {
                ReportError(lexeme, "Missing value in []");
                return nullptr;
            }
            if(expectedRight)
            {
                ReportError(lexeme, "Expected right operand");
                return nullptr;
            }
            result->Priority = 40;
            result->NumOfChildren = 0;
            hasLeft = true;
            expectedRight = false;
            break;
        case 204: // (
            if(hasLeft)
            {
                ReportError(lexeme, "No expected left operand for '('");
                return nullptr;
            };
            result->Priority = 20;
            result->NumOfChildren = 1;
            hasLeft = false;
            expectedRight = true;
            break;
        case 205: // )
            if(expectedRight)
            {
                ReportError(lexeme, "Expected right operand");
                return nullptr;
            }
            result->Priority = 40;
            result->NumOfChildren = 0;
            hasLeft = true;
            expectedRight = false;
            break;
        case 200: // {
        case 201: // }
            ReportError(lexeme, "Unexpected '{' or '}' inside the expression. Are you missing ';'?");
            return nullptr;
    }
    result->IsLeftAssociated = IsLeftAssociated(result->Priority);
    return result;
}

bool IsLeftAssociated(int priority)
{
    return !(priority >= 40 || priority <= 20 ||
             priority == 23 || priority == 35);
}
