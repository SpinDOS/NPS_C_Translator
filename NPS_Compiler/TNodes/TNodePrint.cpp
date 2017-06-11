//
// Created by Alexander on 10-Jun-17.
//

#include "TNode.h"

using namespace std;

string GetIndent(int level) {return string(level *2, ' ');}

void PrintChildren(TBranch *branch, int level)
{
    string indent = GetIndent(level + 1);
    for (int i = 0; i < branch->Children.count(); i++)
    {
        TNode *child = branch->Children.get(i);
        if (child)
            child->Print(level + 1);
        else
            cout << indent << "(null)" << endl;
    }
}

void TBranch::Print(int level)
{
    cout <<  GetIndent(level) << (Lexeme? string(*Lexeme) : "(null)") <<  endl;
    PrintChildren(this, level);
}

void TTypeCast::Print(int level)
{
    cout <<  GetIndent(level) << '(' << this->TargetType->toString() << ')' <<  endl;
    PrintChildren(this, level);
}

void TFunctionCall::Print(int level)
{
    string indent = GetIndent(level);
    cout << indent << "Call function: " <<  endl;
    this->FunctionToCall->Print(level + 1);
    if (this->Children.count() > 0)
    {
        cout << indent << "With parameters: " << endl;
        PrintChildren(this, level);
    }
    else
        cout << indent << "Without parameters" << endl;
}

void TVariableDeclaration::Print(int level)
{
    cout << GetIndent(level) << "Declare variable " << this->Lexeme->lexeme <<
         " of type " << this->DeclaringType->toString();
    if (this->Array_length != nullptr)
    {
        cout << " with length: " << endl;
        this->Array_length->Print(level + 1);
    }
    else
        cout << endl;
    PrintChildren(this, level);
}

void TFunctionDefinition::Print(int level)
{
    cout << GetIndent(level) << "Declare function " << this->Lexeme->lexeme << ": " << endl;
    this->Implementation->Print(level + 1);
}

void TLeaf::Print(int level)
{ cout << GetIndent(level) << this->Lexeme->lexeme << endl; }

void TSwitchCase::Print(int level)
{
    cout << GetIndent(level) << "Switch case: ";
    if (isDefault)
        cout << "default: ";
    else
        cout << "case " << caseNum << ": ";
    cout << "line " << lineNum << endl;
}

void TKeywordNew::Print(int level)
{
    cout << GetIndent(level) << "New " << this->DeclaringType->toString();
    if (this->Array_length != nullptr)
    {
        cout << " with length: " << endl;
        this->Array_length->Print(level + 1);
    }
    else
        cout << endl;
    PrintChildren(this, level);
}