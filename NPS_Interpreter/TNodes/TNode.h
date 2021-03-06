#ifndef NPS_INTERPRETER_TNODES_H
#define NPS_INTERPRETER_TNODES_H

#include <iostream>
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../../NPS_library/collection_containers/list.h"
#include "../../NPS_library/InterpreterTNodeType.h"

namespace NPS_Interpreter
{
    TSimpleLinkedList<char> *GlobalParameters();

    struct TNode{
        int size = 0;
        virtual char* Exec() = 0;
        virtual void free_my_mem(void *mem){}
        void ExecAndFree() { free_my_mem(Exec()); }
    };

    struct TConstant : TNode{
        char *data = nullptr;
        char* Exec() final;
    };

    struct TVariable : TNode{
        char *name = nullptr;
        char* Exec() final;
    };

    struct TDeclaration : TNode{
        char *name = nullptr;
        int underlying_size = 0;
        TNode *arrayLength = nullptr;
        char* Exec();
    };

    struct TFunctionParamsGetter : TDeclaration{
        char* Exec() final;
    };

    struct TBranch : TNode{
        TSimpleLinkedList<TNode> children;
    };

    struct TList : TBranch {
        char* Exec();
    };

    struct TFunctionDefinition : TBranch{
        char* name = nullptr;
        char* Exec() final;
    };

    struct TOperation : TBranch{
        bool need_to_free_my_mem = true;
        int operands_size = 0;
        void free_my_mem(void *mem) override
            {if (need_to_free_my_mem) Heap::free_mem(mem);}
    };
    
    struct TAssignationOperation : TOperation
    { TAssignationOperation() {need_to_free_my_mem = false;} };
    
    struct TFunction : TOperation {
        TFunction(bool return_non_void)
            {need_to_free_my_mem = return_non_void;}
        char* Exec() final;
    };

    struct TSwitchCase : TNode
    {
        bool is_default = false;
        int case_num = 0;
        int line_num = -1;
        char* Exec() final{return nullptr;} // not used
    };
}

#endif //NPS_INTERPRETER_TNODES_H
