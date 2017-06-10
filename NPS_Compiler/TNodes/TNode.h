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
#include "../Operations/FunctionsManager.h"
#include "../ErrorReporter/ErrorReporter.h"

namespace NPS_Compiler
{
    struct TNode
    {
    public:
        TNode (LexemeWord *lexeme) {Lexeme = lexeme;}
        LexemeWord *Lexeme = nullptr;
        TBranch *Parent = nullptr;
        NPS_Interpreter::InterpreterTNodeType InterpreterType =
            NPS_Interpreter::InterpreterTNodeType::NotDefined;
        
        ResultType* GetType() { return _type? _type : _type = getType(); }
        virtual void Print(int level) = 0;
        virtual void Serialize(TiXmlElement* parent) {}
        virtual bool IsConstantValue() {return true;}
    protected:
        virtual ResultType *getType() = 0;
    private:
        ResultType *_type = nullptr; // initialized after getType
    };

    struct TBranch : public TNode
    {
        TBranch(LexemeWord *lexeme) : TNode(lexeme){}
        int Priority = -1;
        bool IsLeftAssociated = false;
        TSimpleLinkedList<TNode> Children;
        void Print(int level);
    };

    struct TOperation : public TBranch
    {
        TOperation(LexemeWord *lexeme) : TBranch(lexeme){}
        int NumOfChildren = -1;
    protected:
        ResultType* getType();
    };
    
    struct TTypeCast final: public TOperation
    {
        TTypeCast(ResultType *targetType, LexemeWord *lexeme);
        ResultType *TargetType = nullptr;
        void Print(int level);
    protected:
        ResultType* getType();
    };
    
    struct TFunctionCall final: public TBranch
    {
        static TFunctionCall* Create_abstract_function();
        TFunctionCall(LexemeWord *lexeme) : TBranch(lexeme)
        {
            Priority = MINPRIORITY; // not used
            IsLeftAssociated = true; // not used
            InterpreterType = NPS_Interpreter::InterpreterTNodeType::FunctionCall;
        }
        TNode *FunctionToCall = nullptr;
        void Print(int level);
    protected:
        ResultType* getType();
    };
    
    struct TTopPriority : public TBranch
    {
        TTopPriority(LexemeWord *lexeme) : TBranch(lexeme)
        {
            IsLeftAssociated = true;
            Priority = 100;
        }
    };

    struct TList final: public TTopPriority
    {
        TList(LexemeWord *lexeme) : TTopPriority(lexeme)
        {
            Heap::free_mem(Lexeme->lexeme);
            Lexeme->lexeme = copy_string("{}");
            InterpreterType = NPS_Interpreter::InterpreterTNodeType::ListOfSentences;
        }
    protected:
        ResultType* getType();
    };
    
    struct TKeyword final: public TTopPriority
    {
        TKeyword(LexemeWord *lexeme) : TTopPriority(lexeme) { }
    protected:
        ResultType* getType();
    };
    
    struct TDeclaration : public TTopPriority
    {
        TDeclaration(LexemeWord *lexeme) : TTopPriority(lexeme) { }
        ResultType *DeclaringType = nullptr;
    protected:
        ResultType* getType() {return DeclaringType;}
    };
    
    struct TVariableDeclaration final: public TDeclaration
    {
        TVariableDeclaration(LexemeWord *lexeme) : TDeclaration(lexeme) { }
        TNode *Array_length = nullptr;
        void Print(int level);
    };
    
    struct TFunctionDefinition final: public TDeclaration
    {
        TFunctionDefinition(LexemeWord *lexeme) : TFunctionDefinition(lexeme) { }
        
        ResultType *ReturnType = nullptr;
        TSimpleLinkedList<FunctionParameterInfo> Parameters;
        TList *Implementation = nullptr;
        
        void Print(int level);
    protected:
        ResultType* getType();
    };
    
    
    struct TLeaf : public TNode
    {
        TLeaf(LexemeWord *lexeme) : TNode(lexeme){}
        void Print(int level);
    };
    
    struct TConstant final: public TLeaf
    {
        TConstant(LexemeWord *lexeme) : TLeaf(lexeme)
        { InterpreterType = NPS_Interpreter::InterpreterTNodeType::Constant;}
        ResultType *constantType = nullptr;
        void *Data = nullptr;
    protected:
        ResultType* getType() final {return constantType;}
    };
    
    struct TVariable final : public TLeaf
    {
        TVariable(LexemeWord *lexeme) : TLeaf(lexeme) { }
    protected:
        ResultType* getType();
    };
    
    
    struct TSwitchCase : public TLeaf
    {
        TSwitchCase(LexemeWord *lexeme) : TLeaf(lexeme)
        { InterpreterType = NPS_Interpreter::InterpreterTNodeType::SwitchCase;}
        bool isDefault = false;
        int caseNum = -1;
        int lineNum = -1;
        void Print(int level);
        bool operator== (TSwitchCase &right)
            {return isDefault || right.isDefault?
                    isDefault == right.isDefault:
                    caseNum == right.caseNum;}
    protected:
        ResultType* getType() final { return nullptr; }
    };
    
    struct TFictiveRoot final: public TTopPriority
    {
        TFictiveRoot() : TTopPriority(nullptr)
        {
            this->Lexeme = reinterpret_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
            this->Lexeme->code = 200; // {
            this->Lexeme->lexeme = copy_string("(fictive root)");
            this->Lexeme->positionInTheText = -1;
        }
        ~TFictiveRoot()
        {
            Heap::free_mem(this->Lexeme->lexeme);
            Heap::free_mem(this->Lexeme);
        }
    protected:
        ResultType* getType() { return nullptr; }
    };
    
    
    TOperation *GetTOperation(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
    TLeaf *GetTLeaf(LexemeWord *lexeme, bool &hasLeft, bool &expectedRight);
    
    void GetOverloads(TNode *node, TSimpleLinkedList<KeyValuePair<char, Func>> *overloads);
    void FreeOverloads(TSimpleLinkedList<KeyValuePair<char, Func>> *overloads);
}

#endif //NPS_C_TRANSLATOR_TNODE_H
