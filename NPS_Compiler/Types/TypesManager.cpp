//
// Created by Alexander on 02-Apr-17.
//

#include "../../NPS_library/heap/heap.h"
#include "TypesManager.h"
#include "../../NPS_library/utils/string_utils.h"

TypesManager::TypesCollection TypesManager::typesCollection;

const TypeInfo* TypesManager::TypesCollection::GetTypeInfo(const char *type)
{
    TypeInfo *result = get(type);
    return error()? nullptr : result;
}

TypesManager::TypesCollection::TypesCollection() :
        THashTable<TypeInfo*>(5,5,5)
{
    TypeInfo *type = new TypeInfo("double");
    type->size = sizeof(double);
    put(type->type(), type);
    type = new TypeInfo("int");
    type->size = sizeof(int);
    put(type->type(), type);
    type = new TypeInfo("char");
    type->size = sizeof(char);
    put(type->type(), type);
    type = new TypeInfo("bool");
    type->size = sizeof(bool);
    put(type->type(), type);
    type = new TypeInfo("void");
    type->size = 0;
    put(type->type(), type);
}

TypesManager::TypesCollection::~TypesCollection()
{
    for (int i = 0; i < element_count; i++)
    {
        List *l = table[i];
        if (l == nullptr)
            continue;
        for (int j = 0; j < l->count(); j++)
        {
            TypeInfo *temp = static_cast<TypeInfo*>(l->get(j));
            temp->~TypeInfo();
        }
    }
}

const TypeInfo * TypesManager::GetTypeInfo(const char *type)
{return typesCollection.GetTypeInfo(type);}

void TypesManager::AddTypeInfo(TypeInfo *typeInfo)
{typesCollection.AddTypeInfo(typeInfo);}