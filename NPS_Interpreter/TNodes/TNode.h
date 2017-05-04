#ifndef NPS_INTERPRETER_TNODES_H
#define NPS_INTERPRETER_TNODES_H

#include <iostream>
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../../NPS_library/collection_containers/list.h"
#include "../../NPS_library/InterpreterTNodeType.h"

namespace NPS_Interpreter
{
    struct ReturnResult
    {
        ReturnResult(char *_data, bool _need_to_free_mem)
        {data = _data; need_to_free_mem = _need_to_free_mem;}
        ReturnResult()
        {data = nullptr; need_to_free_mem = false;}
        char *data;
        bool need_to_free_mem;
    };
    TypeList<ReturnResult> *GlobalParameters();

    struct TNode{
        virtual ReturnResult Exec() = 0;
    };

    struct TConstant : TNode{
        char *data = nullptr;
        ReturnResult Exec() final;
    };

    struct TVariable : TNode{
        char *name = nullptr;
        ReturnResult Exec() final;
    };

    struct TDeclaration : TNode{
        char *name = nullptr;
        int size = 0;
        TNode *arrayLength = nullptr;
        ReturnResult Exec();
    };

    struct TFunctionParamsGetter : TDeclaration{
        ReturnResult Exec() final;
    };

    struct TBranch : TNode{
        TSimpleLinkedList<TNode> children;
    };

    struct TList : TBranch {
        ReturnResult Exec() final;
    };

    struct TFunction : TBranch{
        ReturnResult Exec() final;
    };

    struct TFunctionDefinition : TBranch{
        char* name = nullptr;
        ReturnResult Exec() final;
    };

    struct TOperation : TBranch{
        int size = 0;
    };

    struct TSwitchCase : TNode
    {
        TBranch *parent = nullptr;
        bool is_default = false;
        int case_num = 0;
        int line_num = -1;
        ReturnResult Exec() final;
    };
}

#endif //NPS_INTERPRETER_TNODES_H
