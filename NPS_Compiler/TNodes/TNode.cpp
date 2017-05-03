//
// Created by Alexander on 27-Mar-17.
//

#include <iostream>
#include "TNode.h"
#include "../Variables/VariableTable.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../TypeCast/TypeCastManager.h"
#include "../Operations/OperationsManager.h"

using namespace std;
using namespace NPS_Compiler;

TTypeCast::TTypeCast(ResultType *_targetType, LexemeWord *Lexeme) : TOperation(Lexeme)
{
    this->tNodeType = TNodeTypeCast;
    this->IsLeftAssociated = false;
    this->Priority = 23;
    this->NumOfChildren = 1;
    this->lexeme = Lexeme;
    this->targetType = _targetType;
}

void TOperation::check_changable()
{
    if (this->lexeme->code == 218 && this->children.count() == 1) // *
        return;
    if (this->lexeme->code != 240 && this->lexeme->code != 242) // ?: ,
    {
        ReportError(this->lexeme, "The result of the operation is not assignable");
        return;
    }
    for (int i = this->lexeme->code == 240 ? 1 : 0; i < this->children.count(); i++)
    {
        TNode *node = this->children.get(i);
        if (node->tNodeType == TNodeTypeVariable || node->tNodeType == TNodeTypeDeclaration)
            continue;
        if (node->tNodeType == TNodeTypeOperation)
        {
            static_cast<TOperation *>(node)->check_changable();
            if (ErrorReported())
                return;
            continue;
        }
        ReportError(node->lexeme, "Expression is not assignable");
        return;
    }
}

ResultType* TOperation::_getType()
{
    for(int i = 0; i < this->children.count(); i++)
    {
        ResultType *resultType = this->children.get(i)->getType();
        if (ErrorReported())
            return nullptr;
        if (resultType == nullptr && this->lexeme->code != 241) // =
        {
            ReportError(this->lexeme, "Specify concrete function overload with type cast");
            return nullptr;
        }
    }
    if (this->lexeme->code == 241) // =
    { // find overload for =
        ResultType *left = this->children.getFirst()->getType();
        if (left == nullptr)
        {
            ReportError(this->lexeme, "Can not assign system function");
            return nullptr;
        }
        if (this->children.getLast()->getType() == nullptr)
        {
            TSimpleLinkedList<KeyValuePair<char, Func>> overloads;
            GetOverloads(this->children.getLast(), &overloads);
    
            int i = 0;
            if (left->baseType->typeOfType == PrimCustFunc::Function && left->p_count == 0)
            {
                for (; i < overloads.count(); i++)
                    if (*left->baseType == *overloads.get(i)->value)
                        break;
                if (i == overloads.count())
                {
                    ReportError(this->lexeme, "Can not find overload to match the left operand type");
                    return nullptr;
                }
            }
            Heap::free_mem(this->children.getLast()->lexeme->lexeme);
            this->children.getLast()->lexeme->lexeme = copy_string(overloads.get(i)->key);
            FreeOverloads(&overloads);
        }
    }
    // at this moment there is no 'null' arguments
    return OperationsManager::GetResultOfOperation(this);
}

ResultType* TTypeCast::_getType()
{
    TNode *source = this->children.getFirst();
    if (ErrorReported())
        return nullptr;
    if (source->getType() == nullptr)
    {
        if (ErrorReported())
            return nullptr;
        TSimpleLinkedList<KeyValuePair<char, Func>> overloads;
        GetOverloads(source, &overloads);
        int i = 0;
        ResultType resultType;
        if (targetType->baseType->typeOfType == PrimCustFunc::Function && targetType->p_count == 0)
        {
            for (; i < overloads.count(); i++)
            {
                KeyValuePair<char, Func> *pair = overloads.get(i);
                resultType.baseType = pair->value;
                if (resultType == *targetType)
                    break;
            }
        }
        else
        {
            resultType.baseType = overloads.getFirst()->value;
            if (!TypeCastManager::CanCast(&resultType, targetType, true))
                i = overloads.count();
        }
        if (i == overloads.count())
        {
            ReportError(source->lexeme, "Can not find overload to cast to this type");
            return nullptr;
        }
        Heap::free_mem(source->lexeme->lexeme);
        source->lexeme->lexeme = copy_string(overloads.get(i)->key);
        FreeOverloads(&overloads);
    }
    // this call will set up intepreterTNodeType
    TypeCastManager::Cast(source, this->targetType, true);
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
    // save var overloads to another variable to work with params
    TSimpleLinkedList<KeyValuePair<char, Func>> function_overloads;
    GetOverloads(this->function, &function_overloads);
    // need to find good overload
    int best_matching_i = -1, best_dismatch = this->children.count() + 1;
    // find best matching overload
    for (int i = 0; i < function_overloads.count(); i++)
    {
        Func *overload = function_overloads.get(i)->value;
        if (overload->parameters.count() != this->children.count())
            continue;
        int cur_dismatch = 0;
        for (int j = 0; cur_dismatch >= 0 && j < this->children.count(); j++) // check params
        {
            TNode *actualNode = this->children.get(j);
            ResultType *expected = overload->parameters.get(j),
                    *actual = actualNode->getType();
            if (actual == nullptr)
            {
                if (expected->p_count > 0) // use function when pointer expected
                {
                    cur_dismatch = -1;
                    break;
                }
                TSimpleLinkedList<KeyValuePair<char, Func>> parameter_overloads;
                GetOverloads(actualNode, &parameter_overloads);
                bool found = false; // found overload for param
                for (int k = 0; !found && k < parameter_overloads.count(); k++)
                {
                    ResultType temp;
                    temp.baseType = parameter_overloads.get(k)->value;
                    if (temp == *expected)
                        found = true;
                    else if (TypeCastManager::CanCast(&temp, expected, false))
                    {
                        cur_dismatch++;
                        found = true;
                    }
                }
                FreeOverloads(&parameter_overloads);
                if (!found) // no overload to match the param
                    cur_dismatch = -1;
                continue;
            }
            if (*actual == *expected)
                continue;
            if (TypeCastManager::CanCast(actualNode, expected, false))
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
    KeyValuePair<char, Func> *goodPair = function_overloads.get(best_matching_i);
    Heap::free_mem(this->function->lexeme->lexeme);
    this->function->lexeme->lexeme = copy_string(goodPair->key);
    Func *result = goodPair->value;
    for (int i = 0; i < result->parameters.count(); i++)
    {
        ResultType *paramType = this->children.get(i)->getType();
        if (paramType == nullptr)
        {
            TSimpleLinkedList<KeyValuePair<char, Func>> parameter_overloads;
            GetOverloads(this->children.get(i), &parameter_overloads);
            int j;
            for (j = 0; j < parameter_overloads.count(); j++)
            {
                ResultType temp;
                temp.baseType = parameter_overloads.get(j)->value;
                if (TypeCastManager::CanCast(&temp, result->parameters.get(i), false))
                    break;
            }
            Heap::free_mem(this->children.get(i)->lexeme->lexeme);
            this->children.get(i)->lexeme->lexeme = copy_string(parameter_overloads.get(j)->key);
            FreeOverloads(&parameter_overloads);
        }
        TypeCastManager::Cast(this->children.get(i), result->parameters.get(i), false);
    }
    
    // restore var overloads
    FreeOverloads(&function_overloads);
    return result->returnValue;
}

ResultType* TList::_getType()
{
    VariableTable::PushVisibilityArea();
    for (int i = 0; i < this->children.count(); i++)
    {
        this->children.get(i)->getType();
        if (ErrorReported())
            return nullptr;
    }
    VariableTable::PopVisibilityArea();
    this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ListOfSentences;
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
            if (VariableTable::GetVariableType("break") == nullptr)
                ReportError(this->lexeme, "Can not break here");
            this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordBreak;
            break;
        case 305: // continue
            if (VariableTable::GetVariableType("continue") == nullptr)
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
        return resultType;
    string name = string(this->lexeme->lexeme) + "#0";
    if (VariableTable::GetVariableType(name.c_str()) == nullptr)
        ReportError(this->lexeme, "Variable is not declared");
    return nullptr;
}

ResultType* TDeclaration::_getType()
{
    if (arrayLength != nullptr)
    {
        ResultType *type = arrayLength->getType();
        if (ErrorReported())
            return nullptr;
        LexemeWord rootLexeme;
        rootLexeme.code = 200;
        TOperation root(&rootLexeme);
        root.children.add(arrayLength);
        arrayLength->parent = &root;
        TypeCastManager::Cast(arrayLength, TypesManager::Int(), false);
        if (ErrorReported())
            return nullptr;
        arrayLength->parent = nullptr;
        this->arrayLength = root.children.takeFirst();
    }
    this->intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Declaration;
    VariableTable::AddVariable(lexeme, type);
    if (ErrorReported())
        return nullptr;
    return type;
}

ResultType* TFunctionDefinition::_getType()
{
    if (this->signature->returnValue != TypesManager::Void())
    {
        int i;
        for (i = 0; i < this->implementation->children.count(); i++)
            if (this->implementation->children.get(i)->lexeme->code == 323) // return
                break;
        if (i == this->implementation->children.count())
        {
            ReportError(this->lexeme, "Function does not have 'return' statement");
            return nullptr;
        }
    }
    VariableTable::PushVisibilityArea();
    VariableTable::AddFictiveVariable("return", this->signature->returnValue);
    ResultType *resultType = this->implementation->getType();
    VariableTable::PopVisibilityArea();
    return resultType;
}

void NPS_Compiler::GetOverloads(TNode *node, TSimpleLinkedList<KeyValuePair<char, Func>> *overloads)
{
    for (int i = 0; ; i++)
    {
        string name = string(node->lexeme->lexeme) + "#" + to_string(i);
        ResultType *resultType = VariableTable::GetVariableType(name.c_str());
        if (resultType == nullptr)
            return;
        KeyValuePair<char, Func> *pair = static_cast<KeyValuePair<char, Func> *>
        (Heap::get_mem(sizeof(KeyValuePair<char, Func>)));
        pair->key = copy_string(name.c_str());
        pair->value = static_cast<Func *>(resultType->baseType);
        overloads->add(pair);
    }
}

void NPS_Compiler::FreeOverloads(TSimpleLinkedList<KeyValuePair<char, Func>> *overloads)
{
    while (overloads->count() > 0)
    {
        KeyValuePair<char, Func> *pair = overloads->takeFirst();
        Heap::free_mem(pair->key);
        Heap::free_mem(pair);
    }
}

void TLeaf::Print(int level)
{
    cout << string(level * 2, ' ') << lexeme->lexeme << endl;
}

void TDeclaration::Print(int level)
{
    cout << string(level * 2, ' ') << type->toString() << ' ' << lexeme->lexeme << endl;
    if (arrayLength != nullptr)
        arrayLength->Print(level + 1);
}

void TBranch::Print(int level)
{
    string str(level * 2, ' ');
    cout << string(level * 2, ' ') << (lexeme? string(*lexeme) : "(null)") <<  endl;
    if (this->tNodeType == TNodeTypeFunction)
        static_cast<TFunction*>(this)->function->Print(level+1);
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
    cout << "line " << lineNum << ' ' << this->intepreterTNodeType << endl;
}

void TFunctionDefinition::Print(int level)
{
    cout << string(level * 2, ' ') << "function " << this->lexeme->lexeme <<
         ": " << ' ' << this->intepreterTNodeType << endl;
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
    if (ErrorReported())
        return;
    if (actual != nullptr)
    {
        TypeCastManager::Cast(node->children.getFirst(), returnType, false);
        return;
    }
    else // find good overload
    {
        if (returnType->p_count != 0 || returnType->baseType->typeOfType != PrimCustFunc::Function)
        {
            ReportError(node->children.getFirst()->lexeme, "Function does not return a function");
            return;
        }
        TSimpleLinkedList<KeyValuePair<char, Func>> overloads;
        GetOverloads(node->children.getFirst(), &overloads);
        for (int i = 0; i < overloads.count(); i++)
        {
            KeyValuePair<char, Func> *pair = overloads.get(i);
            if (*pair->value == *returnType->baseType)
            {
                TNode *variable = node->children.getFirst();
                Heap::free_mem(variable->lexeme->lexeme);
                variable->lexeme->lexeme = copy_string(pair->key);
                FreeOverloads(&overloads);
                return;
            }
        }
        ReportError(node->children.getFirst()->lexeme, "Can not find matching overload");
    }
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
        ReportError(condition->lexeme, "Can not use function in condition");
        return;
    }
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
