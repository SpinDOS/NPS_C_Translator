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
        virtual char* Exec() = 0;
        bool need_to_free_my_mem = false;
        void free_my_mem(void *mem)
        {if (need_to_free_my_mem) Heap::free_mem(mem);}
        void ExecAndFree()
        {
            char *data = this->Exec();
            if (need_to_free_my_mem)
                Heap::free_mem(data);
        }
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
        int size = 0;
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

    struct TFunction : TBranch{
        TFunction() : params_sizes(5){}
        TypeList<int> params_sizes;
        char* Exec() final;
    };

    struct TFunctionDefinition : TBranch{
        char* name = nullptr;
        char* Exec() final;
    };

    struct TOperation : TBranch{
        TOperation() {need_to_free_my_mem = true;}
        int size = 0;
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
