//
// Created by Alexander on 18-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_TYPECASTMANAGER_H
#define NPS_C_TRANSLATOR_TYPECASTMANAGER_H

#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

class TypeCastManager
{
public:
    static void Init();
    static bool CanCast(TNode *node, ResultType *targetType, bool explicitCast);
    static void Cast(TNode *node, ResultType *targetType, bool explicitCast);
};

#endif //NPS_C_TRANSLATOR_TYPECASTMANAGER_H
