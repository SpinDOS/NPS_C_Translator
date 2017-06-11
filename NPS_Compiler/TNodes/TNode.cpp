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

TTypeCast::TTypeCast(ResultType *targetType, LexemeWord *lexeme) : TOperation(lexeme)
{
    this->IsLeftAssociated = false;
    this->Priority = 23;
    this->NumOfChildren = 1;
    this->Lexeme = lexeme;
    this->TargetType = targetType;
}

TFunctionCall* TFunctionCall::Create_abstract_function()
{
    LexemeWord *lexemeWord = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
    lexemeWord->code = 205; // )
    lexemeWord->lexeme = copy_string("()");
    lexemeWord->positionInTheText = -1;
    return new TFunctionCall(lexemeWord);
}
//
//ResultType* TOperation::getType()
//{
//    for(int i = 0; i < this->Children.count(); i++)
//    {
//        ResultType *resultType = this->Children.get(i)->GetType();
//        if (ErrorReported())
//            return nullptr;
//        if (resultType == nullptr && this->Lexeme->code != 241) // =
//        {
//            ReportError(this->Lexeme, "Specify concrete function overload with type cast");
//            return nullptr;
//        }
//    }
//    if (this->Lexeme->code == 241) // =
//    { // find overload for =
//        ResultType *left = this->Children.getFirst()->GetType();
//        if (left == nullptr)
//        {
//            ReportError(this->Lexeme, "Can not assign system function");
//            return nullptr;
//        }
//        if (this->Children.getLast()->GetType() == nullptr)
//        {
//            TSimpleLinkedList<KeyValuePair<char, Func>> overloads;
//            GetOverloads(this->Children.getLast(), &overloads);
//
//            int i = 0;
//            if (left->baseType->typeOfType == PrimCustFunc::Function && left->p_count == 0)
//            {
//                for (; i < overloads.count(); i++)
//                    if (*left->baseType == *overloads.get(i)->value)
//                        break;
//                if (i == overloads.count())
//                {
//                    ReportError(this->Lexeme, "Can not find overload to match the left operand type");
//                    return nullptr;
//                }
//            }
//            Heap::free_mem(this->Children.getLast()->Lexeme->lexeme);
//            this->Children.getLast()->Lexeme->lexeme = copy_string(overloads.get(i)->key);
//            FreeOverloads(&overloads);
//        }
//    }
//    // at this moment there is no 'null' arguments
//    return OperationsManager::GetResultOfOperation(this);
//}
//
//ResultType* TTypeCast::getType()
//{
//    TNode *source = this->Children.getFirst();
//    if (ErrorReported())
//        return nullptr;
//    if (source->GetType() == nullptr)
//    {
//        if (ErrorReported())
//            return nullptr;
//        TSimpleLinkedList<KeyValuePair<char, Func>> overloads;
//        GetOverloads(source, &overloads);
//        int i = 0;
//        ResultType resultType;
//        if (TargetType->baseType->typeOfType == PrimCustFunc::Function && TargetType->p_count == 0)
//        {
//            for (; i < overloads.count(); i++)
//            {
//                KeyValuePair<char, Func> *pair = overloads.get(i);
//                resultType.baseType = pair->value;
//                if (resultType == *TargetType)
//                    break;
//            }
//        }
//        else
//        {
//            resultType.baseType = overloads.getFirst()->value;
//            if (!TypeCastManager::CanCast(&resultType, TargetType, true))
//                i = overloads.count();
//        }
//        if (i == overloads.count())
//        {
//            ReportError(source->Lexeme, "Can not find overload to cast to this type");
//            return nullptr;
//        }
//        Heap::free_mem(source->Lexeme->lexeme);
//        source->Lexeme->lexeme = copy_string(overloads.get(i)->key);
//        this->InterpreterType = NPS_Interpreter::InterpreterTNodeType::CastPointerToPointer;
//        FreeOverloads(&overloads);
//    }
//    // this call will set up InterpreterType
//    TypeCastManager::Cast(source, this->TargetType, true);
//    if (ErrorReported())
//        return nullptr;
//    return TargetType;
//}
//
//ResultType* TFunctionCall::getType()
//{
//    // get parameters types
//    for (int i = 0; i < this->Children.count(); i++)
//    {
//        this->Children.get(i)->GetType();
//        if (ErrorReported())
//            return nullptr;
//    }
//    ResultType *varResult = this->FunctionToCall->GetType(); // variable or expression
//    if (ErrorReported())
//        return nullptr;
//    if (varResult != nullptr) // var of type function exists
//        if (varResult->p_count == 0 && varResult->baseType->typeOfType == PrimCustFunc::Function)
//            return static_cast<Func*>(varResult->baseType)->returnValue;
//        else
//        {
//            ReportError(this->FunctionToCall->Lexeme, "Expression does not target function");
//            return nullptr;
//        }
//    // save var overloads to another variable to work with params
//    TSimpleLinkedList<KeyValuePair<char, Func>> function_overloads;
//    GetOverloads(this->FunctionToCall, &function_overloads);
//    // need to find good overload
//    int best_matching_i = -1, best_dismatch = this->Children.count() + 1;
//    // find best matching overload
//    for (int i = 0; i < function_overloads.count(); i++)
//    {
//        Func *overload = function_overloads.get(i)->value;
//        if (overload->parameters.count() != this->Children.count())
//            continue;
//        int cur_dismatch = 0;
//        for (int j = 0; cur_dismatch >= 0 && j < this->Children.count(); j++) // check params
//        {
//            TNode *actualNode = this->Children.get(j);
//            ResultType *expected = overload->parameters.get(j),
//                    *actual = actualNode->GetType();
//            if (actual == nullptr)
//            {
//                if (expected->p_count > 0) // use function when pointer expected
//                {
//                    cur_dismatch = -1;
//                    break;
//                }
//                TSimpleLinkedList<KeyValuePair<char, Func>> parameter_overloads;
//                GetOverloads(actualNode, &parameter_overloads);
//                bool found = false; // found overload for param
//                for (int k = 0; !found && k < parameter_overloads.count(); k++)
//                {
//                    ResultType temp;
//                    temp.baseType = parameter_overloads.get(k)->value;
//                    if (temp == *expected)
//                        found = true;
//                    else if (TypeCastManager::CanCast(&temp, expected, false))
//                    {
//                        cur_dismatch++;
//                        found = true;
//                    }
//                }
//                FreeOverloads(&parameter_overloads);
//                if (!found) // no overload to match the param
//                    cur_dismatch = -1;
//                continue;
//            }
//            if (*actual == *expected)
//                continue;
//            if (TypeCastManager::CanCast(actualNode, expected, false))
//                cur_dismatch++;
//            else
//                cur_dismatch = -1;
//        }
//        if (cur_dismatch < 0) // can not use this overload, try next
//            continue;
//        if (cur_dismatch < best_dismatch) // save this overload
//        {
//            best_dismatch = cur_dismatch;
//            best_matching_i = i;
//        }
//    }
//    if (best_matching_i < 0)
//    {
//        ReportError(this->FunctionToCall->Lexeme, "Can not find overload with the following arguments");
//        return nullptr;
//    }
//    // use found overload
//    KeyValuePair<char, Func> *goodPair = function_overloads.get(best_matching_i);
//    Heap::free_mem(this->FunctionToCall->Lexeme->lexeme);
//    this->FunctionToCall->Lexeme->lexeme = copy_string(goodPair->key);
//    Func *result = goodPair->value;
//    for (int i = 0; i < result->parameters.count(); i++)
//    {
//        ResultType *paramType = this->Children.get(i)->GetType();
//        if (paramType == nullptr)
//        {
//            TSimpleLinkedList<KeyValuePair<char, Func>> parameter_overloads;
//            GetOverloads(this->Children.get(i), &parameter_overloads);
//            int j;
//            for (j = 0; j < parameter_overloads.count(); j++)
//            {
//                ResultType temp;
//                temp.baseType = parameter_overloads.get(j)->value;
//                if (TypeCastManager::CanCast(&temp, result->parameters.get(i), false))
//                    break;
//            }
//            Heap::free_mem(this->Children.get(i)->Lexeme->lexeme);
//            this->Children.get(i)->Lexeme->lexeme = copy_string(parameter_overloads.get(j)->key);
//            FreeOverloads(&parameter_overloads);
//        }
//        TypeCastManager::Cast(this->Children.get(i), result->parameters.get(i), false);
//    }
//
//    // restore var overloads
//    FreeOverloads(&function_overloads);
//    return result->returnValue;
//}
//
//ResultType* TList::getType()
//{
//    VariableTable::PushVisibilityArea();
//    for (int i = 0; i < this->Children.count(); i++)
//    {
//        this->Children.get(i)->GetType();
//        if (ErrorReported())
//            return nullptr;
//    }
//    VariableTable::PopVisibilityArea();
//    this->InterpreterType = NPS_Interpreter::InterpreterTNodeType::ListOfSentences;
//    return nullptr;
//}
//
//void validate_return(TKeyword *node);
//void validate_do(TKeyword *node);
//void validate_while(TKeyword *node);
//void validate_for(TKeyword *node);
//void validate_if(TKeyword *node);
//void validate_switch(TKeyword *node);
//
//ResultType* TKeyword::getType()
//{
//    switch (this->Lexeme->code)
//    {
//        case 323: // return
//            validate_return(this);
//            break;
//        case 301: // break
//            if (VariableTable::GetVariableType("break") == nullptr)
//                ReportError(this->Lexeme, "Can not break here");
//            this->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordBreak;
//            break;
//        case 305: // continue
//            if (VariableTable::GetVariableType("continue") == nullptr)
//                ReportError(this->Lexeme, "Can not continue here");
//            this->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordContinue;
//            break;
//        case 308: // do
//            validate_do(this);
//            break;
//        case 330: // while
//            validate_while(this);
//            break;
//        case 312: // for
//            validate_for(this);
//            break;
//        case 313: // if
//            validate_if(this);
//            break;
//        case 326: //switch
//            validate_switch(this);
//            break;
//        default:
//            ReportError(this->Lexeme, "Type validation for this keyword is not supported yet");
//    }
//    return nullptr;
//}
//
//ResultType* TVariable::getType()
//{
//    // FIX HERE FOR CLASSES, FUNCTION MAY CONTAIN .
//    this->InterpreterType = NPS_Interpreter::InterpreterTNodeType::Variable;
//    ResultType *resultType = VariableTable::GetVariableType(this->Lexeme->lexeme);
//    if (resultType != nullptr)
//        return resultType;
//    string name = string(this->Lexeme->lexeme) + "#0";
//    if (VariableTable::GetVariableType(name.c_str()) == nullptr)
//        ReportError(this->Lexeme, "Variable is not declared");
//    return nullptr;
//}
//
//ResultType* TDeclaration::getType()
//{
//    if (arrayLength != nullptr)
//    {
//        ResultType *type = arrayLength->getType();
//        if (ErrorReported())
//            return nullptr;
//        LexemeWord rootLexeme;
//        rootLexeme.code = 200;
//        TOperation root(&rootLexeme);
//        root.Children.add(arrayLength);
//        arrayLength->parent = &root;
//        TypeCastManager::Cast(arrayLength, TypesManager::Int(), false);
//        if (ErrorReported())
//            return nullptr;
//        arrayLength->parent = nullptr;
//        this->arrayLength = root.Children.takeFirst();
//    }
//    this->InterpreterType = NPS_Interpreter::InterpreterTNodeType::Declaration;
//    VariableTable::AddVariable(Lexeme, type);
//    if (ErrorReported())
//        return nullptr;
//    return type;
//}
//
//ResultType* TFunctionDefinition::getType()
//{
//    if (this->signature->returnValue != TypesManager::Void())
//    {
//        int i;
//        for (i = 0; i < this->Implementation->Children.count(); i++)
//            if (this->Implementation->Children.get(i)->Lexeme->code == 323) // return
//                break;
//        if (i == this->Implementation->Children.count())
//        {
//            ReportError(this->Lexeme, "Function does not have 'return' statement");
//            return nullptr;
//        }
//    }
//    VariableTable::PushVisibilityArea();
//    VariableTable::AddFictiveVariable("return", this->signature->returnValue);
//    ResultType *resultType = this->Implementation->GetType();
//    VariableTable::PopVisibilityArea();
//    return resultType;
//}
//
//void NPS_Compiler::GetOverloads(TNode *node, TSimpleLinkedList<KeyValuePair<char, Func>> *overloads)
//{
//    for (int i = 0; ; i++)
//    {
//        string name = string(node->Lexeme->lexeme) + "#" + to_string(i);
//        ResultType *resultType = VariableTable::GetVariableType(name.c_str());
//        if (resultType == nullptr)
//            return;
//        KeyValuePair<char, Func> *pair = reinterpret_cast<KeyValuePair<char, Func> *>
//        (Heap::get_mem(sizeof(KeyValuePair<char, Func>)));
//        pair->key = copy_string(name.c_str());
//        pair->value = static_cast<Func *>(resultType->baseType);
//        overloads->add(pair);
//    }
//}
//
//void NPS_Compiler::FreeOverloads(TSimpleLinkedList<KeyValuePair<char, Func>> *overloads)
//{
//    while (overloads->count() > 0)
//    {
//        KeyValuePair<char, Func> *pair = overloads->takeFirst();
//        Heap::free_mem(pair->key);
//        Heap::free_mem(pair);
//    }
//}
//
//void validate_return(TKeyword *node)
//{
//    node->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordReturn;
//    ResultType *returnType = VariableTable::GetVariableType("return");
//    if (*returnType == *TypesManager::Void())
//        if (node->Children.count() > 0)
//            ReportError(node->Lexeme, "Function must return void");
//        else
//            return;
//    if (node->Children.count() == 0)
//    {
//        ReportError(node->Lexeme, "Function must return non-void value");
//        return;
//    }
//    ResultType *actual = node->Children.getFirst()->GetType();
//    if (ErrorReported())
//        return;
//    if (actual != nullptr)
//    {
//        TypeCastManager::Cast(node->Children.getFirst(), returnType, false);
//        return;
//    }
//    else // find good overload
//    {
//        if (returnType->p_count != 0 || returnType->baseType->typeOfType != PrimCustFunc::Function)
//        {
//            ReportError(node->Children.getFirst()->Lexeme, "Function does not return a function");
//            return;
//        }
//        TSimpleLinkedList<KeyValuePair<char, Func>> overloads;
//        GetOverloads(node->Children.getFirst(), &overloads);
//        for (int i = 0; i < overloads.count(); i++)
//        {
//            KeyValuePair<char, Func> *pair = overloads.get(i);
//            if (*pair->value == *returnType->baseType)
//            {
//                TNode *variable = node->Children.getFirst();
//                Heap::free_mem(variable->Lexeme->lexeme);
//                variable->Lexeme->lexeme = copy_string(pair->key);
//                FreeOverloads(&overloads);
//                return;
//            }
//        }
//        ReportError(node->Children.getFirst()->Lexeme, "Can not find matching overload");
//    }
//}
//
//void validate_loop(TNode *loop)
//{
//    if (loop == nullptr)
//        return;
//    VariableTable::PushVisibilityArea();
//    ResultType notUsed;
//    VariableTable::AddFictiveVariable("continue", &notUsed);
//    VariableTable::AddFictiveVariable("break", &notUsed);
//    loop->GetType();
//    VariableTable::PopVisibilityArea();
//}
//
//void validate_condition(TNode *condition)
//{
//    ResultType *cond_type = condition->GetType();
//    if (ErrorReported())
//        return;
//    if (cond_type == nullptr)
//    {
//        ReportError(condition->Lexeme, "Can not use function in condition");
//        return;
//    }
//    TypeCastManager::Cast(condition, TypesManager::Bool(), false);
//}
//
//void validate_do(TKeyword *node)
//{
//    validate_condition(node->Children.getFirst());
//    if (ErrorReported())
//        return;
//    validate_loop(node->Children.getLast());
//    if (ErrorReported())
//        return;
//    node->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordDoWhile;
//}
//
//void validate_while(TKeyword *node)
//{
//    validate_condition(node->Children.getFirst());
//    if (ErrorReported())
//        return;
//    validate_loop(node->Children.getLast());
//    if (ErrorReported())
//        return;
//    node->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordWhile;
//}
//
//void validate_if(TKeyword *node)
//{
//    validate_condition(node->Children.getFirst());
//    if (ErrorReported())
//        return;
//
//    if (node->Children.get(1) != nullptr)
//        node->Children.get(1)->GetType();
//    if (ErrorReported())
//        return;
//
//    if (node->Children.getLast() != nullptr)
//        node->Children.getLast()->GetType();
//    if (ErrorReported())
//        return;
//
//    node->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordIf;
//}
//
//void validate_for(TKeyword *node)
//{
//    VariableTable::PushVisibilityArea();
//    ResultType notUsed;
//    VariableTable::AddFictiveVariable("continue", &notUsed);
//    VariableTable::AddFictiveVariable("break", &notUsed);
//
//    TBranch *body = static_cast<TBranch*>(node->Children.getLast());
//    if (body == nullptr || body->tNodeType != TNodeTypeList) // no body or simple expression
//    { // validate (xxx; xxx; xxx) part
//        for (int i = 0; i < 3; i++)
//        {
//            TNode *subnode = node->Children.get(i);
//            if (subnode == nullptr)
//                continue;
//            subnode->GetType();
//            if (ErrorReported())
//                return;
//        }
//        if (body != nullptr)
//            body->GetType();
//        if (ErrorReported())
//            return;
//    }
//    else
//    {// body exists, add (xxx; xxx; xxx) parts to body
//        int count_of_for_parts = 0;
//        for (int i = 0; i < 3; i++) // move for parts to body
//        {
//            TNode *subnode = node->Children.get(i);
//            if (subnode == nullptr)
//                continue;
//            subnode->Parent = body;
//            body->Children.insertBefore(subnode, count_of_for_parts++);
//        }
//        body->GetType();
//        for (int i = 0; i < count_of_for_parts; i++) // return for parts back from body
//            body->Children.takeFirst()->Parent = node;
//        if (ErrorReported())
//            return;
//    }
//    VariableTable::PopVisibilityArea();
//
//    TNode *condition = node->Children.get(1);
//    if (condition != nullptr)
//        validate_condition(condition);
//    if (ErrorReported())
//        return;
//    node->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordFor;
//}
//
//void validate_switch(TKeyword *node)
//{
//    TNode *condition = node->Children.getFirst();
//    ResultType *cond_type = condition->GetType();
//    if (ErrorReported())
//        return;
//    if (cond_type == nullptr)
//    {
//        ReportError(condition->Lexeme, "Switch condition must return int");
//        return;
//    }
//    TypeCastManager::Cast(condition, TypesManager::Int(), false);
//    if (ErrorReported())
//        return;
//
//    if (node->Children.get(1) != nullptr)
//    {
//        VariableTable::PushVisibilityArea();
//        ResultType notUsed;
//        VariableTable::AddFictiveVariable("break", &notUsed);
//        node->Children.get(1)->GetType();
//        VariableTable::PopVisibilityArea();
//        if (ErrorReported())
//            return;
//    }
//    node->InterpreterType = NPS_Interpreter::InterpreterTNodeType::KeywordSwitch;
//}
