//
// Created by Alexander on 27-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_TNODE_H
#define NPS_C_TRANSLATOR_TNODE_H

#define MAXPRIORITY 40
#define MINPRIORITY 20

namespace NPS_Compiler
{
    struct TBranch;
}

#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../LexemeParsing/LexemeParser.h"
#include "../Variables/VariableTable.h"

enum TNodeType
{
    TList,
    TVariable,
    TConstant,
    TFunction,
    TDeclaration,
};

namespace NPS_Compiler
{
    struct TNode
    {
    public:
        LexemeWord *lexeme = nullptr;
        TBranch *parent = nullptr;
        TNodeType tNodeType;
        ResultType *getType() { return type? type : type = _getType(); }
        virtual void Print(int level) = 0;
    protected:
        virtual ResultType *_getType() = 0;
    private:
        ResultType *type = nullptr; // initialized after _getType
    };

    struct TBranch : public TNode
    {
        int Priority;
        bool IsLeftAssociated;
        TSimpleLinkedList<TNode *> children;
        void Print(int level) final;
    };

    struct TOperation : public TBranch
    {
        int NumOfChildren;
        ResultType* _getType() final;
    };
    
    struct TFunction : public TBranch
    {
    
    };

    struct TList : public TBranch
    {
    public:
        TList() {Priority = -10000; IsLeftAssociated = false;}
        ResultType* _getType() final;
    };

    
    struct TLeaf : public TNode
    {
        void Print(int level) final;
    };
    
    struct TConstant final : public TLeaf
    {
        TConstant(){tNodeType = TNodeType::TConstant;}
        ResultType *constantType;
        void *data;
        ResultType *_getType() final {return getType();}
    };
    
    struct TVariable final : public TLeaf
    {
        TVariable(){tNodeType = TNodeType::TVariable;}
        const char *var;
        ResultType *_getType() final
        { return VariableTable::GetVariableType(lexeme); }
    };

    struct TDeclaration : public TLeaf
    {
        ResultType *type;
        char *var;
        ResultType *_getType() final { return type; }
    };
    
    TOperation *GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
    TLeaf *GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
}

#endif //NPS_C_TRANSLATOR_TNODE_H
