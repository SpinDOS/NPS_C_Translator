//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_VARIABLETYPE_H
#define NPS_C_TRANSLATOR_VARIABLETYPE_H

#include <cstring>
#include "../../NPS_library/heap/heap.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"

enum PrimCustFunc
{ Primitive, Custom, Function, };

struct BaseType
{
    BaseType(PrimCustFunc _type) : typeOfType(_type){}
    PrimCustFunc typeOfType;
    bool operator==(BaseType &right)
    { return typeOfType == right.typeOfType? compare(right) : false; }
    bool operator != (BaseType &right) {return !operator==(right);}
protected:
    virtual bool compare(BaseType &right) = 0;
};

struct ResultType
{
    BaseType *baseType = nullptr;
    int p_count = 0;
    bool operator==(const ResultType &right)
    {return p_count == right.p_count && *baseType == *right.baseType;}
    bool operator!=(const ResultType &right)
    {return !operator==(right);}
};

struct VarType : public BaseType
{
    char *type;
    VarType(PrimCustFunc _type) : BaseType(_type){}
protected:
    bool compare(BaseType &right) final
    { return strcmp(this->type, static_cast<VarType*>(&right)->type) == 0; }
};

struct PrimitiveType : public VarType
{ PrimitiveType() : VarType(Primitive){} };

struct CustomType : public VarType
{ CustomType() : VarType(Custom){} };

struct Func : public BaseType
{
    Func() : BaseType(Function){}
    ResultType *returnValue = nullptr;
    TSimpleLinkedList<ResultType> parameters;
protected:
    bool compare(BaseType &right) final
    {
        Func *r = static_cast<Func *>(&right);
        if (parameters.count() != r->parameters.count() ||
                *returnValue != *r->returnValue)
            return false;
        for (int i = 0; i < parameters.count(); i++)
            if (*parameters.get(i) != *r->parameters.get(i))
                return false;
        return true;
    }
};

#endif //NPS_C_TRANSLATOR_VARIABLETYPE_H
