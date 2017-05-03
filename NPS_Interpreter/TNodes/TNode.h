#ifndef NPS_INTERPRETER_TNODES_H
#define NPS_INTERPRETER_TNODES_H

#include <iostream>
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../../NPS_library/TinyXmlLibrary/tinyxml.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../../NPS_library/InterpreterTNodeType.h"

namespace NPS_Interpreter
{
    struct TBranch;

    struct TNode{
        TBranch *parent = nullptr;
        virtual char* Exec() = 0;
    };

    struct TBranch : TNode{
        TSimpleLinkedList<TNode> children;
    };

    struct TOperation : TBranch{
        int size = -1;
        InterpreterTNodeType method = InterpreterTNodeType::NotDefined;
        char* Exec();
    };

    struct TConstant : TNode{
        char *data = nullptr;
        char* Exec(){return data;}
    };

    struct TVariable : TNode{
        char* key = nullptr;
        char* Exec();
    };

    struct TDeclaration : TNode{
        char* key = nullptr;
        int size = 0;
        TNode *arrayLength = nullptr;
        char* Exec();
    };

    struct TFunctionParamsGetter : TDeclaration{

    };

    struct TFunctionDefinition : TBranch{
        char* name = nullptr;
        char* Exec();
    };

    struct TKeyword : TBranch{
        char* keyword = nullptr;
        char* Exec();
    };

    struct TList : TBranch {
        char* Exec();
    };

    struct TFunction : TBranch{
        char* Exec();
    };
}



#endif //NPS_INTERPRETER_TNODES_H
