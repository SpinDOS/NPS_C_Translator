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

TBranch* NPS_Compiler::GetTBranch(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight)
{
    TBranch* result = (TBranch*)Heap::get_mem(sizeof(TBranch));
    switch (lexeme->code)
    {
        case 214: // !
        case 215: // ~
            if (hasLeft) {
                ReportError("error", "No expected left operand (~!)");
                return nullptr;
            }
            result->Priority = 3;
            result->NumOfChildren = 1;
            expectedRight = true;
            hasLeft = false;
            cout << "Unary ~!" << endl;
            break;
        case 218: // *
        case 219: // /
        case 220: // %
            if (!hasLeft) {
                ReportError("error", "expected left operand (* % /)");
                return nullptr;
            }
            result->Priority = 5;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary * % /" << endl;
            break;
        case 221: // + -
        case 222:
            if (hasLeft) {
                result->Priority = 6;
                result->NumOfChildren = 2;
                hasLeft = false;
                expectedRight = true;
                cout << "Binary plus/minus" << endl;
            } else {
                result->Priority = 3;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
                cout << "Unary plus/minus" << endl;
            }
            break;
        case 202: // ++ --
        case 203:
            if (hasLeft) {
                result->Priority = 2;
                result->NumOfChildren = 1;
                expectedRight = false;
                cout << "Postfix++/--" << endl;
            } else {
                result->Priority = 3;
                result->NumOfChildren = 1;
                hasLeft = false;
                expectedRight = true;
                cout << "Prefix ++/--" << endl;
            }
            break;
        case 241: // =
            if (!hasLeft) {
                ReportError("error", "Expected left operand (=)");
                return nullptr;
            }
            result->Priority = 15;
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
                ReportError("error", "Expected left operand ( < > <= >=");
                return nullptr;
            }
            result->Priority = 8;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Logical < < <= >=" << endl;
            break;
        case 234: // &
            if (hasLeft) {
                result->Priority = 10;
                result->NumOfChildren = 2;
                expectedRight = true;
                hasLeft = false;
                cout << "Bit &" << endl;
            } else {
                result->Priority = 3;
                result->NumOfChildren = 1;
                expectedRight = true;
                hasLeft = false;
                cout << "Unary &" << endl;
            }
            break;
        case 237: // ^
            if (!hasLeft) {
                ReportError("error", "Expected left operand ^");
                return nullptr;
            }
            result->Priority = 11;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary xor ^" << endl;
            break;
        case 236: // |
            if (!hasLeft) {
                ReportError("error", "Expected left operand |");
                return nullptr;
            }
            result->Priority = 12;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary |" << endl;
            break;
        case 238: // ||
            if (!hasLeft) {
                ReportError("error", "Expected left operand ||");
                return nullptr;
            }
            result->Priority = 14;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary ||" << endl;
            break;
        case 242: // ,
            if (!hasLeft) {
                ReportError("error", "Expected left operand ,");
                return nullptr;
            }
            result->Priority = 17;
            result->NumOfChildren = 2;
            hasLeft = false;
            expectedRight = true;
            cout << "Binary ," << endl;
            break;
        case 239: // ?
        case 240:  // :
            if (!hasLeft) {
                ReportError("error", "Expected left operand ?:");
                return nullptr;
            }
            result->Priority = 15;
            result->NumOfChildren = 3;
            expectedRight = true;
            hasLeft = false;
            cout << "Ternary ?:" << endl;
            break;
        case 229: // ==
        case 233: // !=
            if (!hasLeft) {
                ReportError("error", "Expected left operand != ==");
                return nullptr;
            }
            result->Priority = 9;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary logical operation == != " << endl;
            break;
        case 208: // .
        case 209: // ->
            if (!hasLeft) {
                ReportError("error", "Expected left operand as object ->");
                return nullptr;
            }
            result->Priority = 2;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary -> ." << endl;
            break;
        case 223: // <<
        case 224: // >>
            if (!hasLeft) {
                ReportError("error", "Expected left operand << >>");
                return nullptr;
            }
            result->Priority = 7;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary shift << >>" << endl;
            break;
        case 235: // &&
            if (!hasLeft) {
                ReportError("error", "Expected left operand &&");
                return nullptr;
            }
            result->Priority = 13;
            result->NumOfChildren = 2;
            expectedRight = true;
            hasLeft = false;
            cout << "Binary && " << endl;
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