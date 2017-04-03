//
// Created by Alexander on 02-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_TYPESMANAGER_H
#define NPS_C_TRANSLATOR_TYPESMANAGER_H

#include "TypeInfo.h"
#include "../../NPS_library/collection_containers/THashTable.h"

class TypesManager
{
    struct TypesCollection : THashTable<TypeInfo*>
    {
        TypesCollection();
        ~TypesCollection();
        const TypeInfo* GetTypeInfo(const char *type);
        void AddTypeInfo(TypeInfo *typeInfo){put(typeInfo->type(), typeInfo);}
    };
    static TypesCollection typesCollection;
public:
    static const TypeInfo* GetTypeInfo(const char *type);
    static void AddTypeInfo(TypeInfo *typeInfo);
};


#endif //NPS_C_TRANSLATOR_TYPESMANAGER_H
