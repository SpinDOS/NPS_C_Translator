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
        TBranch *parent;

        virtual char* Exec() = 0;
    };

    struct TBranch : TNode{
        TSimpleLinkedList<TNode> *children;
    };

    struct TOperation : TBranch{
        int size = -1;
        InterpreterTNodeType method;
        char* Exec();
    };

    struct TConstant : TNode{
        char *data;
        char* Exec(){return data;}
    };

    struct TVariable : TNode{
        char* key;
        char* Exec();
    };

    struct TDeclaration : TNode{
        char* key;
        int size;
        char* Exec();
    };

    struct TFunctionParamsGetter : TDeclaration{

    };

    struct TFunctionDefinition : TBranch{
        char* name;
        char* Exec();
    };

    struct TKeyword : TBranch{
        char* keyword;
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
