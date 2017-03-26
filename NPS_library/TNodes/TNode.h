//
// Created by Alexander on 27-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_TNODE_H
#define NPS_C_TRANSLATOR_TNODE_H

#include "../collection_containers/list.h"
#include "../../NPS_Compiler/LexemeParsing/LexemeParser.h"

struct TNode
{
    virtual void *Exec() = 0;
    TNode *parent = nullptr;
    int priority;
    TypeList<TNode*> children;
};

struct TConstant : public TNode
{
    TConstant() : TNode(){priority = 100;}
    void *data;
    virtual void *Exec(){return data;}
};

struct TOperation : public TNode
{
    int type_of_operation;
    virtual void *Exec();
};

TNode* GetTNode(LexemeWord *lexeme);

#endif //NPS_C_TRANSLATOR_TNODE_H
