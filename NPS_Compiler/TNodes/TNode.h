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
    TNodeTypeList,
    TNodeTypeVariable,
    TNodeTypeConstant,
    TNodeTypeOperation,
    TNodeTypeFunction,
    TNodeTypeDeclaration,
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
        TOperation() {tNodeType = TNodeTypeOperation;}
        int NumOfChildren;
        ResultType* _getType() final;
    };
    
    struct TFunction : public TBranch
    {
        TFunction()
        {
            tNodeType = TNodeTypeFunction;
            Priority = MINPRIORITY; // not used
            IsLeftAssociated = true; // not used
        }
        ResultType* _getType() final{throw "Not implemented";}
    };

    struct TList : public TBranch
    {
    public:
        TList() {tNodeType = TNodeTypeList;}
        ResultType* _getType() final;
    };

    
    struct TLeaf : public TNode
    {
        void Print(int level) final;
    };
    
    struct TConstant final : public TLeaf
    {
        TConstant(){tNodeType = TNodeTypeConstant;}
        ResultType *constantType;
        void *data;
        ResultType *_getType() final {return getType();}
    };
    
    struct TVariable final : public TLeaf
    {
        TVariable(){tNodeType = TNodeTypeVariable;}
        const char *var;
        ResultType *_getType() final
        { return VariableTable::GetVariableType(lexeme); }
    };

    struct TDeclaration : public TLeaf
    {
        TDeclaration() {tNodeType = TNodeTypeDeclaration;}
        ResultType *type;
        char *var;
        ResultType *_getType() final { return type; }
    };
    
    TOperation *GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
    TLeaf *GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
}

#endif //NPS_C_TRANSLATOR_TNODE_H
