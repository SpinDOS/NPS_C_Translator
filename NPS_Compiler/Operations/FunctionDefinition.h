//
// Created by Alexander on 18-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_FUNCTIONSIGNATURE_H
#define NPS_C_TRANSLATOR_FUNCTIONSIGNATURE_H

#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "../TNodes/ResultType.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

struct FunctionDefinition
{
    ResultType *returnValue;
    LexemeWord *name;
    TSimpleLinkedList<ResultType> paramTypes;
    TSimpleLinkedList<char*> paramNames;
    TNode *implementation = nullptr;

    bool operator==(FunctionDefinition &right)
    {
        if (strcmp(name->lexeme, right.name->lexeme) != 0)
            return false;
        int count = paramTypes.count();
        if (count != right.paramTypes.count())
            return false;
        for (int i = 0; i < count; i++)
            if (*paramTypes.get(i) != *right.paramTypes.get(i))
                return false;
        return true;
    }
    bool operator!= (FunctionDefinition &right){return !operator==(right);}
};

#endif //NPS_C_TRANSLATOR_FUNCTIONSIGNATURE_H
