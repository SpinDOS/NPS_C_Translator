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

ResultType* TBranch::_getType()
{
    ResultType *result = PrimitiveOperationsManager::GetResultOfOperation(this);
    if (result)
        return result;
    return CustomOperationsManager::GetResultOfOperation(this);
}

TLeaf* NPS_Compiler::GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(lexeme->start, "Operation expected");
        return nullptr;
    }
    hasLeft = true;
    expectedRight = false;
    if (400 <= lexeme->code && lexeme->code < 600) // variable name
    {
        TVariable *variable = new TVariable;
        variable->lexeme = lexeme;
        variable->var = copy_string(lexeme->start, lexeme->length);
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

TBranch* NPS_Compiler::GetTBranch(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    TBranch* result = (TBranch*)Heap::get_mem(sizeof(TBranch));
    switch (lexeme->code)
    {
        case 214: // !
        case 215: // ~
            if (hasLeft) {
                ReportError(lexeme->start, "No expected left operand (~!)");
                return nullptr;
            }
            result->Priority = 23;
            result->NumOfChildren = 1;
            expectedRight = true;
            hasLeft = false;
            cout << "Unary ~!" << endl;
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
                ReportError(lexeme->start, "expected left operand (% /)");
                return nullptr;
            }
            result->Priority = 25;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary % /" << endl;
            break;
        case 221: // + -
        case 222:
            if (hasLeft) {
                result->Priority = 26;
                result->NumOfChildren = 2;
                hasLeft = false;
                expectedRight = true;
                cout << "Binary plus/minus" << endl;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
                cout << "Unary plus/minus" << endl;
            }
            break;
        case 202: // ++ --
        case 203:
            if (hasLeft) {
                result->Priority = 22;
                result->NumOfChildren = 1;
                expectedRight = false;
                cout << "Postfix++/--" << endl;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                hasLeft = false;
                expectedRight = true;
                cout << "Prefix ++/--" << endl;
            }
            break;
        case 241: // =
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand (=)");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary =" << endl;
            break;
        case 225: // <
        case 226: // <=
        case 227: // >
        case 228: // >=
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand ( < > <= >=");
                return nullptr;
            }
            result->Priority = 28;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Logical < < <= >=" << endl;
            break;
        case 234: // &
            if (hasLeft) {
                result->Priority = 30;
                result->NumOfChildren = 2;
                expectedRight = true;
                hasLeft = false;
                cout << "Bit &" << endl;
            } else {
                result->Priority = 23;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
                cout << "Unary &" << endl;
            }
            break;
        case 237: // ^
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand ^");
                return nullptr;
            }
            result->Priority = 31;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary xor ^" << endl;
            break;
        case 236: // |
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand |");
                return nullptr;
            }
            result->Priority = 32;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary |" << endl;
            break;
        case 238: // ||
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand ||");
                return nullptr;
            }
            result->Priority = 34;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary ||" << endl;
            break;
        case 242: // ,
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand ,");
                return nullptr;
            }
            result->Priority = 37;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary ," << endl;
            break;
        case 239: // ?
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand ?:");
                return nullptr;
            }
            result->Priority = 35;
            result->NumOfChildren = 3;
            expectedRight = true;
            hasLeft = false;
            cout << "Ternary ?:" << endl;
            break;
        case 240:  // :
            result->Priority = 38;
            result->NumOfChildren = 1;
            expectedRight = true;
            hasLeft = false;
            break;
        case 229: // ==
        case 233: // !=
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand != ==");
                return nullptr;
            }
            result->Priority = 29;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary logical operation == != " << endl;
            break;
        case 208: // .
        case 209: // ->
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand as object ->");
                return nullptr;
            }
            result->Priority = 22;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary -> ." << endl;
            break;
        case 223: // <<
        case 224: // >>
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand << >>");
                return nullptr;
            }
            result->Priority = 27;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary shift << >>" << endl;
            break;
        case 235: // &&
            if (!hasLeft) {
                ReportError(lexeme->start, "Expected left operand &&");
                return nullptr;
            }
            result->Priority = 33;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary && " << endl;
            break;
        case 243: // ;
            if(expectedRight)
            {
                ReportError(lexeme->start, "Expected right operand");
                return nullptr;
            }
            result->Priority = 18;
            result->NumOfChildren = 0;
            break;
        case 206: // [
            if(!hasLeft)
            {
                ReportError(lexeme->start, "Expected left operand");
                return nullptr;
            }
            hasLeft = false;
            expectedRight = true;
            result->Priority = 19;
            result->NumOfChildren = 1;
            break;
        case 207: // ]
            if(!hasLeft)
            {
                ReportError(lexeme->start, "Missing value in []");
                return nullptr;
            }
            if(expectedRight)
            {
                ReportError(lexeme->start, "Expected right operand");
                return nullptr;
            }
            expectedRight = false;
            result->Priority = 39;
            result->NumOfChildren = 0;
            break;
        case 204: // (
            if(hasLeft)
            {
                ReportError(lexeme->start, "No expected left operand");
                return nullptr;
            }
            expectedRight = true;
            hasLeft = false;
            result->Priority = 19;
            result->NumOfChildren = 1;
            break;
        case 205: // )
            if(expectedRight)
            {
                ReportError(lexeme->start, "Expected right operand");
                return nullptr;
            }
            hasLeft = true;
            expectedRight = false;
            result->Priority = 39;
            result->NumOfChildren = 0;
            break;
    }
    result->IsLeftAssociated = IsLeftAssociated(result->Priority);
}


bool IsLeftAssociated(int priority)
{
    bool array[17];
    for (int i = 0; i < 17; ++i) {
        array[i] = true;
    }
    array[2] = false;
    array[14] = false;
    if(priority >= 17) return true;
    return array[priority - 1];
}