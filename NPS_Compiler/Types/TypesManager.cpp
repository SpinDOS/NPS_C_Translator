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
    result->baseType = new VarType(type);
    result->baseType->typeOfType = Primitive;
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
    
    // size -1 means that it is a pointer
    type = new TypeInfo("function");
    type->size = -1;
    typesCollection.put(type->type(), type);
    
    type = new TypeInfo("#pointer");
    type->size = -1;
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
    if (type->baseType->typeOfType == PrimCustFunc::Function)
        return true;
    int backup = type->p_count;
    type->p_count = 0;
    bool result = *type == *_bool || *type == *_char ||
            *type == *_int || *type == *_double ||
                              *type == *_void;
    type->p_count = backup;
    return result;
}

const TypeInfo* TypesManager::GetTypeInfo(ResultType *type)
{
    if (type->p_count > 0)
        return typesCollection.get("#pointer");
    else if (type->baseType->typeOfType == PrimCustFunc::Function)
        return typesCollection.get("function");
    else
        return typesCollection.get(type->toString().c_str());
}

ResultType* TypesManager::GetResultType(const char *type)
{return resultTypes.get(type);}

bool TypesManager::AddTypeInfo(TypeInfo *typeInfo)
{
    if (typesCollection.get(typeInfo->type()) != nullptr)
        return false;
    typesCollection.put(typeInfo->type(), typeInfo);
    ResultType *resultType = new ResultType;
    resultType->baseType = new VarType(typeInfo->type());
    resultTypes.put(typeInfo->type(), resultType);
    return true;
}