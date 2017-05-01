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
    static bool ValidateCustomCast(Func *signature, LexemeWord *lexeme, bool explicitCast);
    static bool CanCast(ResultType *from, ResultType *to, bool explicitCast);
    static bool CanCast(TNode *from, ResultType *to, bool explicitCast);
    static void Cast(TNode *node, ResultType *targetType, bool explicitCast);
};

#endif //NPS_C_TRANSLATOR_TYPECASTMANAGER_H
