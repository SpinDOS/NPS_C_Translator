//
// Created by Alexander on 02-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_TYPESMANAGER_H
#define NPS_C_TRANSLATOR_TYPESMANAGER_H

#include "TypeInfo.h"

class TypesManager
{
    static struct TypesCollection
    {
        TypesCollection();
        const TypeInfo* GetTypeInfo(const char *type);
        void AddTypeInfo(const char *type, const TypeInfo &typeInfo);
    } typesCollection;
public:
    static const TypeInfo* GetTypeInfo(const char *type){return typesCollection.GetTypeInfo(type);}
    static void AddTypeInfo(const char *type, const TypeInfo &typeInfo)
    {typesCollection.AddTypeInfo(type, typeInfo);}
};

#endif //NPS_C_TRANSLATOR_TYPESMANAGER_H
