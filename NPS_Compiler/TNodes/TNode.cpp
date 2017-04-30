//
// Created by Alexander on 27-Mar-17.
//

#include <iostream>
#include "TNode.h"
#include "../Operations/PrimitiveOperationsManager.h"
#include "../Variables/VariableTable.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../TypeCast/TypeCastManager.h"

using namespace std;

TTypeCast::TTypeCast(ResultType *_targetType, LexemeWord *Lexeme) : TOperation(Lexeme)
{
    this->tNodeType = TNodeTypeCast;
    this->IsLeftAssociated = false;
    this->Priority = 23;
    this->NumOfChildren = 1;
    this->lexeme = Lexeme;
    this->targetType = _targetType;
}

ResultType* TOperation::_getType()
{

}

ResultType* TTypeCast::_getType()
{
    ResultType *from = this->children.getFirst()->getType();
    if (ErrorReported())
        return nullptr;
    if (from == nullptr)
    {
        ReportError(this->lexeme, "No experession to cast");
        return nullptr;
    }
    TypeCastManager::Cast(this->children.getFirst(), this->targetType, true);
    if (ErrorReported())
        return nullptr;
    return targetType;
}

ResultType* TFunction::_getType()
{

}

ResultType* TList::_getType()
{

}

void validate_return(TKeyword *node);
void validate_do(TKeyword *node);
void validate_while(TKeyword *node);
void validate_for(TKeyword *node);
void validate_if(TKeyword *node);
void validate_switch(TKeyword *node);

ResultType* TKeyword::_getType()
{
    switch (this->lexeme->code)
    {
        case 323: // return
            validate_return(this);
            break;
        case 301: // break
            if (VariableTable::GetVariableType("break"))
                ReportError(this->lexeme, "Can not break here");
            this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordBreak;
            break;
        case 305: // continue
            if (VariableTable::GetVariableType("continue"))
                ReportError(this->lexeme, "Can not continue here");
            this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordContinue;
            break;
        case 308: // do
            validate_do(this);
            break;
        case 330: // while
            validate_while(this);
            break;
        case 312: // for
            validate_for(this);
            break;
        case 313: // if
            validate_if(this);
            break;
        case 326: //switch
            validate_switch(this);
            break;
    }
    return nullptr;
}

ResultType* TVariable::_getType()
{
    ResultType *resultType = VariableTable::GetVariableType(this->lexeme->lexeme);
    if (resultType != nullptr)
        return nullptr;
    string temp = string(this->lexeme->lexeme) + "№0";
    if (VariableTable::GetVariableType(temp.c_str()) == nullptr)
        ReportError(this->lexeme, "Variable is not declared");
    return nullptr;
}

ResultType* TDeclaration::_getType()
{
    VariableTable::AddVariable(lexeme, type);
    return ErrorReported()? nullptr : type;
}





void TLeaf::Print(int level)
{
    string str(level * 2, ' ');
    string lex(*lexeme);
    cout << str << lex << endl;
}

void TDeclaration::Print(int level)
{
    string str(level * 2, ' ');
    string lex(*lexeme);
    cout << str << type->toString() << ' ' << lexeme->lexeme  << endl;
    if (arrayLength != nullptr)
    {
        arrayLength->Print(level + 1);
    }
}

void TBranch::Print(int level)
{
    string str(level * 2, ' ');
    string lex(*lexeme);
    cout << str << lex << endl;
    for (int i = 0; i < children.count(); i++)
    {
        TNode *child = children.get(i);
        if (child)
            child->Print(level + 1);
        else
            cout << str << "  " << "(null)" << endl;
    }
}

void TSwitchCase::Print(int level)
{
    string str(level * 2, ' ');
    if (isDefault)
        cout << str + "default: ";
    else
        cout << str << "case " << caseNum << ": ";
    cout << "line " << lineNum << endl;
}

void TFunctionDefinition::Print(int level)
{
    string str(level * 2, ' ');
    cout << str << "function " << this->lexeme->lexeme << ": " << endl;
    this->implementation->Print(level);
}

void validate_return(TKeyword *node)
{
    ResultType *returnType = VariableTable::GetVariableType("return");
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordReturn;
    if (*returnType == *TypesManager::Void())
        if (node->children.count() > 0)
            ReportError(node->lexeme, "Function must return void");
        else
            return;
    if (node->children.count() == 0)
        ReportError(node->lexeme, "Function returns non-void void");
    ResultType *actual = node->children.getFirst()->getType();
    if (ErrorReported() || *actual == *returnType)
        return;
    TypeCastManager::Cast(node->children.getFirst(), returnType, false);
}

void validate_loop(TNode *loop)
{
    if (loop == nullptr)
        return;
    VariableTable::PushVisibilityArea();
    ResultType notUsed;
    VariableTable::AddFictiveVariable("continue", &notUsed);
    VariableTable::AddFictiveVariable("break", &notUsed);
    loop->getType();
    VariableTable::PopVisibilityArea();
}

void validate_condition(TNode *condition)
{
    ResultType *cond_type = condition->getType();
    if (ErrorReported())
        return;
    if (cond_type == nullptr)
    {
        ReportError(condition->lexeme, "Condition must return bool value");
        return;
    }
    if (*cond_type != *TypesManager::Bool())
        TypeCastManager::Cast(condition, TypesManager::Bool(), false);
}

void validate_do(TKeyword *node)
{
    validate_condition(node->children.getFirst());
    if (ErrorReported())
        return;
    validate_loop(node->children.getLast());
    if (ErrorReported())
        return;
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordDoWhile;
}

void validate_while(TKeyword *node)
{
    validate_condition(node->children.getFirst());
    if (ErrorReported())
        return;
    validate_loop(node->children.getLast());
    if (ErrorReported())
        return;
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordWhile;
}

void validate_if(TKeyword *node)
{
    validate_condition(node->children.getFirst());
    if (ErrorReported())
        return;
    if (node->children.getLast() != nullptr)
        node->children.getLast()->getType();
    if (ErrorReported())
        return;
    if (node->children.count() == 3 && node->children.get(1) != nullptr)
        node->children.get(1)->getType();
    if (ErrorReported())
        return;
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordIf;
}

void validate_for(TKeyword *node)
{
    VariableTable::PushVisibilityArea();
    TList *body = static_cast<TList*>(node->children.getLast());
    if (body == nullptr) // no body
    { // validate only (xxx; xxx; xxx) part
        for (int i = 0; i < 3; i++)
        {
            TNode *subnode = node->children.get(i);
            if (subnode == nullptr)
                continue;
            subnode->getType();
            if (ErrorReported())
                return;
        }
    }
    else
    {// body exists, add (xxx; xxx; xxx) parts to body
        int count_of_for_parts = 0;
        for (int i = 0; i < 3; i++)
        {
            TNode *subnode = node->children.get(i);
            if (subnode == nullptr)
                continue;
            body->children.insertBefore(subnode, count_of_for_parts++);
        }
        ResultType notUsed;
        VariableTable::AddFictiveVariable("continue", &notUsed);
        VariableTable::AddFictiveVariable("break", &notUsed);
        body->getType();
        if (ErrorReported())
            return;
    }
    VariableTable::PopVisibilityArea();
    
    TNode *condition = node->children.get(1);
    if (condition != nullptr)
        validate_condition(condition);
    if (ErrorReported())
        return;
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordFor;
}

void validate_switch(TKeyword *node)
{
    TNode *condition = node->children.getFirst();
    ResultType *cond_type = condition->getType();
    if (ErrorReported())
        return;
    if (cond_type == nullptr)
    {
        ReportError(condition->lexeme, "Switch condition must return int");
        return;
    }
    if (*cond_type != *TypesManager::Int())
        TypeCastManager::Cast(condition, TypesManager::Int(), false);
    if (ErrorReported())
        return;

    if (node->children.get(1) != nullptr)
    {
        VariableTable::PushVisibilityArea();
        ResultType notUsed;
        VariableTable::AddFictiveVariable("break", &notUsed);
        node->children.get(1)->getType();
        VariableTable::PopVisibilityArea();
        if (ErrorReported())
            return;
    }
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordSwitch;
}