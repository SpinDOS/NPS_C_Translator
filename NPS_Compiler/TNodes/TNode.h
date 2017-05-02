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

#include "../LexemeParsing/LexemeParser.h"
#include "ResultType.h"
#include "../../NPS_library/collection_containers/KeyValuePair.h"
#include "../../NPS_library/InterpreterTNodeType.h"
#include "../../NPS_library/TinyXmlLibrary/tinyxml.h"

namespace NPS_Compiler
{
    void clear_var_overloads();
    TSimpleLinkedList<KeyValuePair<char, Func>> *get_var_overloads();
    
    enum TNodeType
    {
        TNodeTypeParamsGetter,
        TNodeTypeList,
        TNodeKeyword,
        TNodeTypeVariable,
        TNodeTypeConstant,
        TNodeTypeDeclaration,
        TNodeTypeFunctionDefinition,
        TNodeTypeOperation,
        TNodeTypeFunction,
        TNodeTypeCast,
        TNodeTypeSwitchCase,
    };

    struct TNode
    {
    public:
        TNode (LexemeWord *Lexeme, TNodeType TNodeType)
        {lexeme = Lexeme; tNodeType = TNodeType;}
        LexemeWord *lexeme = nullptr;
        TBranch *parent = nullptr;
        NPS_Interpreter::InterpreterTNodeType intepreterTNodeType =
            NPS_Interpreter::InterpreterTNodeType::NotDefined;
        
        TNodeType tNodeType;
        ResultType *getType() { clear_var_overloads(); return type? type : type = _getType(); }
        virtual void Print(int level) = 0;
        virtual void Serialize(TiXmlElement* parent){};
    protected:
        virtual ResultType *_getType() = 0;
    private:
        ResultType *type = nullptr; // initialized after _getType
    };

    struct TBranch : public TNode
    {
        TBranch(LexemeWord *Lexeme, TNodeType TNodeType) : TNode(Lexeme, TNodeType){}
        int Priority = -1;
        bool IsLeftAssociated = false;
        TSimpleLinkedList<TNode> children;
        void Print(int level);
    };

    struct TOperation : public TBranch
    {
        TOperation(LexemeWord *Lexeme) : TBranch(Lexeme, TNodeTypeOperation){}
        int NumOfChildren = -1;
        void check_changable();
        int size = 0; // FOR ASSIGN
        void Serialize(TiXmlElement* parent);
    protected:
        ResultType* _getType();
    };
    
    struct TTypeCast : public TOperation
    {
        TTypeCast(ResultType *TargetType, LexemeWord *Lexeme);
        ResultType *targetType = nullptr;
    protected:
        ResultType* _getType() final;
    };
    
    struct TFunction : public TBranch
    {
        TFunction(LexemeWord *Lexeme) : TBranch(Lexeme, TNodeTypeFunction)
        {
            Priority = MINPRIORITY; // not used
            IsLeftAssociated = true; // not used
            intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::FunctionCall;
        }
        void Serialize(TiXmlElement *parent);
        TNode *function = nullptr;
        void Print(int level) final;
    protected:
        ResultType* _getType() final;
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
        void Serialize(TiXmlElement* parent);
    protected:
        ResultType* _getType() final;
    };
    
    struct TKeyword : public TTopPriority
    {
        TKeyword(LexemeWord *Lexeme) : TTopPriority(Lexeme, TNodeKeyword) { }
        void Serialize(TiXmlElement *parent);
    protected:
        ResultType* _getType() final;
    };
    
    struct TLeaf : public TNode
    {
        TLeaf(LexemeWord *Lexeme, TNodeType TNodeType) : TNode(Lexeme, TNodeType){}
        void Print(int level);
    };
    
    struct TConstant final : public TLeaf
    {
        TConstant(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeConstant)
        { intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::Constant;}
        ResultType *constantType = nullptr;
        void *data = nullptr;
        void Serialize(TiXmlElement *parent);
    protected:
        ResultType *_getType() final {return constantType;}
    };
    
    struct TVariable final : public TLeaf
    {
        TVariable(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeVariable) { }
        void Serialize(TiXmlElement *parent);
    protected:
        ResultType *_getType() final;
    };

    struct TDeclaration : public TLeaf
    {
        TDeclaration(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeDeclaration) { }
        ResultType *type = nullptr;
        TNode *arrayLength = nullptr;
        void Print(int level) final;
        void Serialize(TiXmlElement* parent);
    protected:
        ResultType *_getType() final;
    };

    struct TFunctionDefinition : public TLeaf
    {
        TFunctionDefinition(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeFunctionDefinition) { }
        Func *signature = nullptr;
        TList *implementation = nullptr;
        void Print(int level) final;
        void Serialize(TiXmlElement* parent);
    protected:
        ResultType *_getType() final;
    };

    struct TFunctionParamsGetter : public TDeclaration
    {
        TFunctionParamsGetter(LexemeWord *Lexeme) : TDeclaration(Lexeme)
        {
            tNodeType = TNodeTypeParamsGetter;
            intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::ParamsGetter;
        }
        void Serialize(TiXmlElement *parent);
    };
    
    struct TSwitchCase : public TLeaf
    {
        TSwitchCase(LexemeWord *Lexeme) : TLeaf(Lexeme, TNodeTypeSwitchCase)
        { intepreterTNodeType = NPS_Interpreter::InterpreterTNodeType::KeywordSwitchCase;}
        bool isDefault = false;
        int caseNum = -1;
        int lineNum = -1;
        void Print(int level) final;
        bool operator== (TSwitchCase &right)
            {return isDefault || right.isDefault?
                    isDefault == right.isDefault:
                    caseNum == right.caseNum;}

    protected:
        ResultType *_getType() final { return nullptr; }
    };
    
    TOperation *GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
    TLeaf *GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
}

#endif //NPS_C_TRANSLATOR_TNODE_H
