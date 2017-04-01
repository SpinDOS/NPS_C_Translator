//
// Created by Alexander on 27-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_TNODE_H
#define NPS_C_TRANSLATOR_TNODE_H

namespace NPS_Compiler
{
    struct TBranch;
    struct TOperation;
}

#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../LexemeParsing/LexemeParser.h"
#include "../Variables/VariableTable.h"
#include "../Operations/OperationsManager.h"

namespace NPS_Compiler
{
    struct TNode
    {
        LexemeWord *lexeme;
        TBranch *parent = nullptr;
        ResultType *type; // initialized after _getType
        ResultType *getType()
        {return type = _getType();}
    protected:
        virtual ResultType *_getType() = 0;
    };

    struct TBranch : public TNode
    {
        int Priority;
        bool IsLeftAssociated;
        int NumOfChildren;
        TSimpleLinkedList<TNode *> children;
    };
    
    struct TDeclaration : public TBranch
    {   // NOTE: don't forget to hande 'int x + 3;'
        //ResultType *_getType() final
        //{ /*do stuff with child*/ return nullptr;}
    };
    
    struct TOperation : public TBranch
    {
        ResultType *_getType() final
        {return OperationsManager::GetResultOfOperation(this);};
    };
    
    
    struct TLeaf : public TNode { };
    
    struct TConstant final : public TLeaf
    {
        ResultType *type;
        ResultType *_getType() final {return type;}
    };
    
    struct TVariable final : public TLeaf
    {
        const char *var;
        ResultType *_getType() final
        { return VariableTable::GetVariableType(var); }
    };
    
    TBranch *GetTBranch(LexemeWord *lexeme);
    TLeaf *GetTLeaf(LexemeWord *lexeme);
}

#endif //NPS_C_TRANSLATOR_TNODE_H
