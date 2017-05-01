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
#include "../../NPS_library/collection_containers/KeyValuePair.h"

using namespace std;

TSimpleLinkedList<KeyValuePair<char, Func>> functionPretendents;

void clear_pretendents()
{
    while (functionPretendents.count() > 0)
    {
        KeyValuePair<char, Func> *pair = functionPretendents.takeLast();
        Heap::free_mem(pair->key);
        Heap::free_mem(pair);
    }
}

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
    // this call will set up intepreterTNodeType
    TypeCastManager::Cast(this->children.getFirst(), this->targetType, true);
    if (ErrorReported())
        return nullptr;
    return targetType;
}

ResultType* TFunction::_getType()
{
    // get parameters types
    for (int i = 0; i < this->children.count(); i++)
    {
        this->children.get(i)->getType();
        if (ErrorReported())
            return nullptr;
    }
    ResultType *varResult = this->function->getType(); // variable or expression
    if (ErrorReported())
        return nullptr;
    if (varResult != nullptr) // var of type function exists
        if (varResult->p_count == 0 && varResult->baseType->typeOfType == PrimCustFunc::Function)
            return static_cast<Func*>(varResult->baseType)->returnValue;
        else
        {
            ReportError(this->function->lexeme, "Expression does not target function");
            return nullptr;
        }
    // need to find good overload
    int best_matching_i = -1, best_dismatch = this->children.count() + 1;
    // find best matching overload
    for (int i = 0; i < functionPretendents.count(); i++)
    {
        Func *overload = functionPretendents.get(i)->value;
        if (overload->parameters.count() != this->children.count())
            continue;
        int cur_dismatch = 0;
        for (int j = 0; cur_dismatch >= 0 && j < this->children.count(); j++)
        {
            ResultType *expected = overload->parameters.get(j),
                    *actual = this->children.get(j)->getType();
            if (*actual == *expected)
                continue;
            if (TypeCastManager::CanCast(actual, expected, false))
                cur_dismatch++;
            else
                cur_dismatch = -1;
        }
        if (cur_dismatch < 0) // can not use this overload, try next
            continue;
        if (cur_dismatch < best_dismatch) // save this overload
        {
            best_dismatch = cur_dismatch;
            best_matching_i = i;
        }
    }
    if (best_matching_i < 0)
    {
        ReportError(this->function->lexeme, "Can not find overload with the following arguments");
        return nullptr;
    }
    // use found overload
    KeyValuePair<char, Func> *goodPair = functionPretendents.get(best_matching_i);
    Heap::free_mem(this->function->lexeme->lexeme);
    this->function->lexeme->lexeme = copy_string(goodPair->key);
    Func *result = goodPair->value;
    for (int i = 0; i < result->parameters.count(); i++)
        if (*this->children.get(i)->getType() != *result->parameters.get(i))
            TypeCastManager::Cast(this->children.get(i), result->parameters.get(i), false);
    clear_pretendents();
    return result->returnValue;
}

ResultType* TList::_getType()
{
    for (int i = 0; i < this->children.count(); i++)
    {
        this->children.get(i)->getType();
        if (ErrorReported())
            return nullptr;
    }
    this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::TList;
    return nullptr;
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
        default:
            ReportError(this->lexeme, "Type validation for this keyword is not supported yet");
    }
    return nullptr;
}

ResultType* TVariable::_getType()
{
    // FIX HERE FOR CLASSES, FUNCTION MAY CONTAIN .
    this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Variable;
    ResultType *resultType = VariableTable::GetVariableType(this->lexeme->lexeme);
    if (resultType != nullptr)
        return nullptr;
    string name = string(this->lexeme->lexeme) + "№0";
    resultType = VariableTable::GetVariableType(name.c_str());
    if (resultType == nullptr)
        ReportError(this->lexeme, "Variable is not declared");
    // add all overloads with their names to global 'pretendents' list
    if (functionPretendents.count() > 0)
    {
        ReportError(0l, "I can not detect function pretendents list usage");
        return nullptr;
    }
    int i = 0;
    do
    {
        KeyValuePair<char, Func> *pair = static_cast<KeyValuePair<char, Func>*>
            (Heap::get_mem(sizeof(KeyValuePair<char, Func>)));
        pair->key = copy_string(name.c_str());
        pair->value = static_cast<Func*>(resultType->baseType);
        functionPretendents.add(pair);
        name = string(this->lexeme->lexeme) + "№" + to_string(++i);
        resultType = VariableTable::GetVariableType(name.c_str());
    }
    while (resultType != nullptr);
    return nullptr;
}

ResultType* TDeclaration::_getType()
{
    this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Declaration;
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
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordReturn;
    ResultType *returnType = VariableTable::GetVariableType("return");
    if (*returnType == *TypesManager::Void())
        if (node->children.count() > 0)
            ReportError(node->lexeme, "Function must return void");
        else
            return;
    if (node->children.count() == 0)
        ReportError(node->lexeme, "Function returns non-void value");
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
    
    if (node->children.get(1) != nullptr)
        node->children.get(1)->getType();
    if (ErrorReported())
        return;
    
    if (node->children.getLast() != nullptr)
        node->children.getLast()->getType();
    if (ErrorReported())
        return;
    
    node->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordIf;
}

void validate_for(TKeyword *node)
{
    VariableTable::PushVisibilityArea();
    ResultType notUsed;
    VariableTable::AddFictiveVariable("continue", &notUsed);
    VariableTable::AddFictiveVariable("break", &notUsed);
    
    TBranch *body = static_cast<TBranch*>(node->children.getLast());
    if (body == nullptr || body->tNodeType != TNodeTypeList) // no body or simple expression
    { // validate (xxx; xxx; xxx) part
        for (int i = 0; i < 3; i++)
        {
            TNode *subnode = node->children.get(i);
            if (subnode == nullptr)
                continue;
            subnode->getType();
            if (ErrorReported())
                return;
        }
        if (body != nullptr)
            body->getType();
        if (ErrorReported())
            return;
    }
    else
    {// body exists, add (xxx; xxx; xxx) parts to body
        int count_of_for_parts = 0;
        for (int i = 0; i < 3; i++) // move for parts to body
        {
            TNode *subnode = node->children.get(i);
            if (subnode == nullptr)
                continue;
            subnode->parent = body;
            body->children.insertBefore(subnode, count_of_for_parts++);
        }
        body->getType();
        for (int i = 0; i < count_of_for_parts; i++) // return for parts back from body
            body->children.takeFirst()->parent = node;
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