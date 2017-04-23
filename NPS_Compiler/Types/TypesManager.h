//
// Created by Alexander on 02-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_TYPESMANAGER_H
#define NPS_C_TRANSLATOR_TYPESMANAGER_H

#include "TypeInfo.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../TNodes/ResultType.h"

struct TypesManager
{
    static void Init();
    static const TypeInfo* GetTypeInfo(const char *type);
    static void AddTypeInfo(TypeInfo *typeInfo);
    static bool IsType(const char *type);
    static bool IsPrimitive(ResultType *type);
    static ResultType *Bool();
    static ResultType *Char();
    static ResultType *Int();
    static ResultType *Double();
    static ResultType *Void();
    static ResultType* GetResultType(const char *type);
};

#endif //NPS_C_TRANSLATOR_TYPESMANAGER_H
