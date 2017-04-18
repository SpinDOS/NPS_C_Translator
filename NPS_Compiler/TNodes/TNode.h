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
    TNodeTypeParamsGetter,
    TNodeTypeList,
    TNodeKeyword,
    TNodeTypeVariable,
    TNodeTypeConstant,
    TNodeTypeDeclaration,
    TNodeTypeOperation,
    TNodeTypeFunction,
    TNodeTypeCast,
    TNodeTypeSwitchCase,
};

namespace NPS_Compiler
{
    struct TNode
    {
    public:
        TNode (LexemeWord *Lexeme, TNodeType TNodeType)
        {lexeme = Lexeme; tNodeType = TNodeType;}
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
        TBranch(LexemeWord *Lexeme, TNodeType TNodeType) : TNode(Lexeme, TNodeType){}
        int Priority;
        bool IsLeftAssociated;
        TSimpleLinkedList<TNode> children;
        void Print(int level) final;
    };

    struct TOperation : public TBranch
    {
        TOperation(LexemeWord *Lexeme) : TBranch(Lexeme, TNodeTypeOperation){}
        int NumOfChildren;
        ResultType* _getType();
    };
    
    struct TTypeCast : public TOperation
    {
        TTypeCast(LexemeWord *TargetType, int P_count);
        TTypeCast(LexemeWord *TargetType) : TTypeCast(TargetType, 0) { }
        char *targetType;
        int p_count;
    };
    
    struct TFunction : public TBranch
    {
        TFunction(LexemeWord *Lexeme) : TBranch(Lexeme, TNodeTypeFunction)
        {
            Priority = MINPRIORITY; // not used
            IsLeftAssociated = true; // not used
        }
        ResultType* _getType() final{throw "Not implemented";}
    };
    
    struct TTopPriority : public TBranch
    {
        TTopPriority(LexemeWord *Lexeme, TNodeType TNodeType) : TBranch(Lexeme, TNodeType)
        {
            IsLeftAssociated = true;
            Priority = 100;
        }
    };

    struct TList : public TTopPriority
    {
        TList(LexemeWord *Lexeme) : TTopPriority(Lexeme, TNodeTypeList)
        {
            Heap::free_mem(Lexeme->lexeme);
            Lexeme->lexeme = copy_string("{}");
        }
        ResultType* _getType() final;
    };
    
    struct TKeyword : public TTopPriority
    {
        TKeyword(LexemeWord *Lexeme) : TTopPriority(Lexeme, TNodeKeyword) { }
        ResultType* _getType() final { return nullptr; }
    };
    
    struct TLeaf : public TNode
    {
        TLeaf(LexemeWord *Lexeme, TNodeType TNodeType) : TNode(Lexeme, TNodeType){}
        void Print(int level);
    };
    
    struct TConstant final : public TLeaf
    {
        TConstant(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeConstant) { }
        ResultType *constantType;
        void *data;
        ResultType *_getType() final {return getType();}
    };
    
    struct TVariable final : public TLeaf
    {
        TVariable(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeVariable) { }
        const char *var;
        ResultType *_getType() final
        { return VariableTable::GetVariableType(lexeme); }
    };

    struct TDeclaration : public TLeaf
    {
        TDeclaration(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeDeclaration) { }
        ResultType *type;
        TNode *arrayLength;
        ResultType *_getType() final { return type; }
        void Print(int level) final;
    };

    struct TFunctionParamsGetter : public TDeclaration
    {
        TFunctionParamsGetter(LexemeWord *Lexeme) : TDeclaration(Lexeme)
        { tNodeType = TNodeTypeParamsGetter; }
    };
    
    struct TSwitchCase : public TLeaf
    {
        TSwitchCase(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeSwitchCase) { }
        bool isDefault = false;
        int caseNum;
        int lineNum;
        ResultType *_getType() final { return nullptr; }
        void Print(int level) final;
        bool operator== (TSwitchCase &right)
            {return isDefault || right.isDefault?
                    isDefault == right.isDefault:
                    caseNum == right.caseNum;}
    };
    
    TOperation *GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
    TLeaf *GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
}

#endif //NPS_C_TRANSLATOR_TNODE_H
