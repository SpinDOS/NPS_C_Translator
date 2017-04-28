//
// Created by Alexander on 02-Apr-17.
//

#include "../../NPS_library/heap/heap.h"
#include "TypesManager.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

ResultType *_bool;
ResultType *_char;
ResultType *_int;
ResultType *_double;
ResultType *_void;

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
    
    type = new TypeInfo("bool");
    type->size = sizeof(bool);
    typesCollection.put(type->type(), type);
    _bool = CreatePrimitiveResultType("bool");
    resultTypes.put("bool", _bool);
    
    type = new TypeInfo("char");
    type->size = sizeof(char);
    typesCollection.put(type->type(), type);
    _char =  CreatePrimitiveResultType("char");
    resultTypes.put("char", _char);
    
    type = new TypeInfo("int");
    type->size = sizeof(int);
    typesCollection.put(type->type(), type);
    _int = CreatePrimitiveResultType("int");
    resultTypes.put("int", _int);
    
    type = new TypeInfo("double");
    type->size = sizeof(double);
    typesCollection.put(type->type(), type);
    _double = CreatePrimitiveResultType("double");
    resultTypes.put("double", _double);
    
    type = new TypeInfo("void");
    type->size = 0;
    typesCollection.put(type->type(), type);
    _void = CreatePrimitiveResultType("void");
    resultTypes.put("void", _void);
    
    type = new TypeInfo("function");
    type->size = sizeof(void*);
    typesCollection.put(type->type(), type);
}

ResultType* TypesManager::Void()
{return _void;}
ResultType* TypesManager::Bool()
{return _bool;}
ResultType* TypesManager::Char()
{return _char;}
ResultType* TypesManager::Int()
{return _int;}
ResultType* TypesManager::Double()
{return _double;}

bool TypesManager::IsType(const char *type)
{ return typesCollection.get(type) != nullptr; }

bool TypesManager::IsPrimitive(ResultType *type)
{
    return *type == *_bool || *type == *_char ||
            *type == *_int || *type == *_double ||
                              *type == *_void;
}

const TypeInfo * TypesManager::GetTypeInfo(const char *type)
{return typesCollection.get(type);}

ResultType* TypesManager::GetResultType(const char *type)
{return resultTypes.get(type);}

void TypesManager::AddTypeInfo(TypeInfo *typeInfo)
{typesCollection.put(typeInfo->type(), typeInfo);}