//
// Created by Alexander on 04-May-17.
//

#include "OperationManager.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;

bool get_break_or_return()
{
    char *state = VariableTable::GetVariableData("break");
    if (*reinterpret_cast<bool*>(state))
        return true;
    state = VariableTable::GetVariableData("return");
    return *reinterpret_cast<bool*>(state);
}

bool check_condition(TBranch *operation, int index, bool check_break_and_return)
{
    if (check_break_and_return && get_break_or_return())
        return false;
    TNode *node = operation->children.get(index);
    if (!node)
        return true;
    char *condition = node->Exec();
    bool result = *reinterpret_cast<bool*>(condition);
    node->free_my_mem(condition);
    return result;
}

void reinitialize_continue()
{ *reinterpret_cast<bool*>(VariableTable::GetVariableData("continue")) = false; }

void prepare_for_loop()
{
    VariableTable::PushVisibilityArea();
    VariableTable::AddVariable("break", sizeof(bool));
    VariableTable::AddVariable("continue", sizeof(bool));
}

void finish_loop()
{VariableTable::PopVisibilityArea();}

struct TKeywordDoWhile : TBranch
{
    char* Exec() final
    {
        prepare_for_loop();
        TNode *body = this->children.getLast();
        do
        {
            if (body)
            {
                body->ExecAndFree();
                reinitialize_continue();
            }
        }
        while (check_condition(this, 0, true));
        finish_loop();
        return nullptr;
    }
};

struct TKeywordWhile : TBranch
{
    char* Exec() final
    {
        prepare_for_loop();
        TNode *body = this->children.getLast();
        while (check_condition(this, 0, true))
        {
            if (body)
            {
                body->ExecAndFree();
                reinitialize_continue();
            }
        }
        finish_loop();
        return nullptr;
    }
};

struct TKeywordFor : TBranch
{
    char* Exec() final
    {
        prepare_for_loop();
        TNode *pre_body = this->children.getFirst();
        TNode *body = this->children.getLast();
        TNode *post_body = this->children.get(2);
        if (pre_body)
            pre_body->ExecAndFree();
        while (check_condition(this, 1, true))
        {
            if (body)
                body->ExecAndFree();
            if (post_body)
                post_body->ExecAndFree();
            reinitialize_continue();
        }
        finish_loop();
        return nullptr;
    }
};

struct TKeywordIf : TBranch
{
    char* Exec() final
    {
        TNode *node = check_condition(this, 0, false)?
                      this->children.get(1) : this->children.getLast();
        if (node != nullptr)
            node->ExecAndFree();
        return nullptr;
    }
};

struct TKeywordSwitch : TBranch
{
    char* Exec() final
    {
        char* condition_result = this->children.getFirst()->Exec();
        int condition = *reinterpret_cast<int*>(condition_result);
        this->children.getFirst()->free_my_mem(condition_result);
        if (this->children.count() <= 2)
            return nullptr;
        TList *list = static_cast<TList*>(this->children.get(1));
        int line_num = -1;
        for (int i = 2; i < this->children.count(); i++)
        {
            TSwitchCase *switchCase = static_cast<TSwitchCase*>(this->children.get(i));
            if (switchCase->is_default || switchCase->case_num == condition)
            {
                line_num = switchCase->line_num;
                break;
            }
        }
        if (line_num < 0)
            return nullptr;
        
        prepare_for_loop();
        for (int i = line_num; !get_break_or_return() && i < list->children.count(); i++)
            list->children.get(i)->ExecAndFree();
        finish_loop();
        return nullptr;
    }
};

struct TKeywordBreak : TBranch
{
    char* Exec() final
    {
        *reinterpret_cast<bool*>(VariableTable::GetVariableData("break")) = true;
        return nullptr;
    }
} _break;

struct TKeywordContinue : TBranch
{
    char* Exec() final
    {
        *reinterpret_cast<bool*>(VariableTable::GetVariableData("continue")) = true;
        return nullptr;
    }
} _continue;

struct TKeywordReturn : TBranch
{
    char* Exec() final
    {
        *reinterpret_cast<bool*>(VariableTable::GetVariableData("return")) = true;
        if (this->children.count() == 0)
            return nullptr;
        char *result = this->children.getFirst()->Exec();
        char *new_result = Heap::get_mem(this->size);
        memcpy(new_result, result, this->size);
        this->children.getFirst()->free_my_mem(result);
        GlobalParameters()->add(new_result);
        return nullptr;
    }
};

//==========================================================================================

TBranch* OperationManager::GetTKeyword(InterpreterTNodeType type)
{
    switch (type)
    {
        case InterpreterTNodeType::KeywordDoWhile:
            return new TKeywordDoWhile;
        case InterpreterTNodeType::KeywordWhile:
            return new TKeywordWhile;
        case InterpreterTNodeType::KeywordFor:
            return new TKeywordFor;
        case InterpreterTNodeType::KeywordIf:
            return new TKeywordIf;
        case InterpreterTNodeType::KeywordSwitch:
            return new TKeywordSwitch;
        case InterpreterTNodeType::KeywordBreak:
            return &_break;
        case InterpreterTNodeType::KeywordContinue:
            return &_continue;
        case InterpreterTNodeType::KeywordReturn:
            return new TKeywordReturn;
        default:
            return nullptr;
    }
    return nullptr;
}
