//
// Created by Alexander on 27-Mar-17.
//

#include <iostream>
#include "TNode.h"
#include "cstring"
#include "../Operations/PrimitiveOperationsManager.h"
#include "../Operations/CustomOperationsManager.h"
#include "../ErrorReporter/ErrorReporter.h"

using namespace std;

bool IsLeftAssociated(int priority);

ResultType* TOperation::_getType()
{
    ResultType *result = PrimitiveOperationsManager::GetResultOfOperation(this);
    if (result)
        return result;
    return CustomOperationsManager::GetResultOfOperation(this);
}

ResultType* TList::_getType()
{
    for (int i = 0; i < children.count(); i++)
        children.get(i)->getType();
    return nullptr;
}

TLeaf* NPS_Compiler::GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(lexeme->positionInTheText, "Operation expected");
        return nullptr;
    }
    hasLeft = true;
    expectedRight = false;
    if (400 <= lexeme->code && lexeme->code < 600) // variable name
    {
        TVariable *variable = new TVariable;
        variable->lexeme = lexeme;
        variable->var = lexeme->lexeme;
        return variable;
    }
    TConstant *result = new TConstant;
    result->lexeme = lexeme;
    if (lexeme->code == 100) // string constant
    {
        result->constantType = new ResultType("char", 1, true);
        result->data = parse_string_constant(*lexeme);
        return result;
    }
    if (lexeme->code == 110) // char constant
    {
        result->constantType = new ResultType("char");
        result->data = Heap::get_mem(1);
        char temp = parse_char_constant(*lexeme);
        memcpy(result->data, &temp, 1);
        return result;
    }
    if (lexeme->code == 140) // bool constant
    {
        result->constantType = new ResultType("bool");
        result->data = Heap::get_mem(1);
        //char temp = parse_char_constant(*lexeme);
        //memcpy(result->data, &temp, 1);
        return result;
    }
    // numeric constant
    char *type;
    double data = parse_num_constant(*lexeme, &type);
    result->constantType = new ResultType(type);
    result->data = new double;
    memcpy(result->data, &data, sizeof(double));
    Heap::free_mem(type);
    return result;
}

TOperation* NPS_Compiler::GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    TOperation* result = new TOperation;
    result->lexeme = lexeme;
    switch (lexeme->code)
    {
        case 214: // !
        case 215: // ~
            if (hasLeft) {
                ReportError(lexeme->positionInTheText, "No expected left operand (~!)");
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
                result->Priority = 22;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
            }
            break;
        case 219: // /
        case 220: // %
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "expected left operand (% /)");
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
                ReportError(lexeme->positionInTheText, "Expected left operand (=)");
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
                ReportError(lexeme->positionInTheText, "Expected left operand ( < > <= >=");
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
                expectedRight = true;
                hasLeft = false;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
            }
            break;
        case 237: // ^
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand ^");
                return nullptr;
            }
            result->Priority = 31;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            break;
        case 236: // |
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand |");
                return nullptr;
            }
            result->Priority = 32;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 238: // ||
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand ||");
                return nullptr;
            }
            result->Priority = 34;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 242: // ,
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand ,");
                return nullptr;
            }
            result->Priority = 37;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            break;
        case 239: // ?
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand ?:");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 3;
            expectedRight = true;
            hasLeft = false;
            break;
        case 240:  // :
            if(expectedRight)
            {
                ReportError(lexeme->positionInTheText, "Expected right operand");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 1;
            expectedRight = true;
            hasLeft = false;
            break;
        case 229: // ==
        case 233: // !=
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand != ==");
                return nullptr;
            }
            result->Priority = 29;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            break;
        case 208: // .
        case 209: // ->
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand as object ->");
                return nullptr;
            }
            result->Priority = 22;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            break;
        case 223: // <<
        case 224: // >>
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand << >>");
                return nullptr;
            }
            result->Priority = 27;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            break;
        case 235: // &&
            if (!hasLeft) {
                ReportError(lexeme->positionInTheText, "Expected left operand &&");
                return nullptr;
            }
            result->Priority = 33;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            break;
        case 243: // ;
            if(expectedRight)
            {
                ReportError(lexeme->positionInTheText, "Expected right operand");
                return nullptr;
            }
            hasLeft = false;
            expectedRight = false;
            result->Priority = 40;
            result->NumOfChildren = 0;
            break;
        case 206: // [
            if(!hasLeft)
            {
                ReportError(lexeme->positionInTheText, "Expected left operand");
                return nullptr;
            }
            hasLeft = false;
            expectedRight = true;
            result->Priority = 20;
            result->NumOfChildren = 2;
            break;
        case 207: // ]
            if(!hasLeft)
            {
                ReportError(lexeme->positionInTheText, "Missing value in []");
                return nullptr;
            }
            if(expectedRight)
            {
                ReportError(lexeme->positionInTheText, "Expected right operand");
                return nullptr;
            }
            expectedRight = false;
            result->Priority = 40;
            result->NumOfChildren = 0;
            break;
        case 204: // (
            if(hasLeft)
            {
                ReportError(lexeme->positionInTheText, "No expected left operand");
                return nullptr;
            }
            expectedRight = true;
            hasLeft = false;
            result->Priority = 20;
            result->NumOfChildren = 1;
            break;
        case 205: // )
            if(expectedRight)
            {
                ReportError(lexeme->positionInTheText, "Expected right operand");
                return nullptr;
            }
            hasLeft = true;
            expectedRight = false;
            result->Priority = 40;
            result->NumOfChildren = 0;
            break;
    }
    result->IsLeftAssociated = IsLeftAssociated(result->Priority);
}


bool IsLeftAssociated(int priority)
{
    if(priority >= 40 || priority <= 20 ||
            priority == 23 || priority == 35) return false;
    return true;
}

void TLeaf::Print(int level)
{
    string str(level * 2, ' ');
    string lex(*lexeme);
    cout << str << lex << endl;
}

void TBranch::Print(int level)
{
    string str(level * 2, ' ');
    string lex(*lexeme);
    cout << str << lex << endl;
    for (int i = 0; i < children.count(); i++)
        children.get(i)->Print(level+1);
}