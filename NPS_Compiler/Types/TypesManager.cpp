//
// Created by Alexander on 02-Apr-17.
//

#include "../../NPS_library/heap/heap.h"
#include "TypesManager.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

THashTable<TypeInfo> typesCollection(5, 5, 5);
THashTable<ResultType> resultTypes(5, 5, 5);

ResultType *CreatePrimitiveResultType(const char *type)
{
    ResultType *result = new ResultType;
    VarType *varType = new VarType;
    varType->typeOfType = Primitive;
    varType->type = copy_string(type);
    result->baseType = varType;
    return result;
}

void TypesManager::Init()
{
    TypeInfo *type;
    type = new TypeInfo("double");
    type->size = sizeof(double);
    typesCollection.put(type->type(), type);
    resultTypes.put("double", CreatePrimitiveResultType("double"));
    type = new TypeInfo("int");
    type->size = sizeof(int);
    typesCollection.put(type->type(), type);
    resultTypes.put("int", CreatePrimitiveResultType("int"));
    type = new TypeInfo("char");
    type->size = sizeof(char);
    typesCollection.put(type->type(), type);
    resultTypes.put("char", CreatePrimitiveResultType("char"));
    type = new TypeInfo("bool");
    type->size = sizeof(bool);
    typesCollection.put(type->type(), type);
    resultTypes.put("bool", CreatePrimitiveResultType("bool"));
    type = new TypeInfo("void");
    type->size = 0;
    typesCollection.put(type->type(), type);
    resultTypes.put("void", CreatePrimitiveResultType("void"));
    type = new TypeInfo("function");
    type->size = sizeof(void*);
    typesCollection.put(type->type(), type);
}

bool TypesManager::IsType(const char *type)
{ return typesCollection.get(type) != nullptr; }

bool TypesManager::IsPrimitive(const char *type)
{
    return strcmp(type, "int") == 0 || strcmp(type, "bool") == 0 ||
            strcmp(type, "double") == 0 || strcmp(type, "char") == 0 ||
            strcmp(type, "void") == 0 || strcmp(type, "function") == 0;
}

const TypeInfo * TypesManager::GetTypeInfo(const char *type)
{return typesCollection.get(type);}

ResultType* TypesManager::GetResultType(const char *type)
{return resultTypes.get(type);}

void TypesManager::AddTypeInfo(TypeInfo *typeInfo)
{typesCollection.put(typeInfo->type(), typeInfo);}