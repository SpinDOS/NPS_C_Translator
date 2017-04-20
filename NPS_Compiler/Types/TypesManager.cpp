//
// Created by Alexander on 02-Apr-17.
//

#include "../../NPS_library/heap/heap.h"
#include "TypesManager.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

THashTable<TypeInfo> typesCollection(5, 5, 5);

void TypesManager::Init()
{
    TypeInfo *type;
    type = new TypeInfo("double");
    type->size = sizeof(double);
    typesCollection.put(type->type(), type);
    type = new TypeInfo("int");
    type->size = sizeof(int);
    typesCollection.put(type->type(), type);
    type = new TypeInfo("char");
    type->size = sizeof(char);
    typesCollection.put(type->type(), type);
    type = new TypeInfo("bool");
    type->size = sizeof(bool);
    typesCollection.put(type->type(), type);
    type = new TypeInfo("void");
    type->size = 0;
    typesCollection.put(type->type(), type);
    type = new TypeInfo("function");
    type->size = sizeof(TNode *);
    typesCollection.put(type->type(), type);
}

bool TypesManager::IsType(const char *type)
{
    return typesCollection.get(type) != nullptr;
}

bool TypesManager::IsPrimitive(const char *type)
{
    return strcmp(type, "int") == 0 || strcmp(type, "bool") == 0 ||
            strcmp(type, "double") == 0 || strcmp(type, "char") == 0 ||
            strcmp(type, "void") == 0 || strcmp(type, "function") == 0;
}

const TypeInfo * TypesManager::GetTypeInfo(const char *type)
{return typesCollection.get(type);}

void TypesManager::AddTypeInfo(TypeInfo *typeInfo)
{typesCollection.put(typeInfo->type(), typeInfo);}