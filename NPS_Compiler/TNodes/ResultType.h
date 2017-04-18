//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_VARIABLETYPE_H
#define NPS_C_TRANSLATOR_VARIABLETYPE_H

#include "../../NPS_library/heap/heap.h"
#include "../../NPS_library/utils/string_utils.h"

struct ResultType
{
    char *baseType;
    int p_count;
    bool isConst;
    ResultType(const char *_type, int _p_count, bool _is_const)
    {
        baseType = copy_string(_type);
        p_count = _p_count;
        isConst = _is_const;
    }
    ResultType(const char *_type, bool _is_const) : ResultType(_type, 0, _is_const){}
    ResultType(const char *_type) : ResultType(_type, true){}
    ~ResultType()
    {
        Heap::free_mem(baseType);
    }
    bool operator==(const ResultType &t)
    {return p_count == t.p_count && compare_strings(baseType, t.baseType);}
    bool operator!=(const ResultType &t)
    {return !operator==(t);}
};

#endif //NPS_C_TRANSLATOR_VARIABLETYPE_H
