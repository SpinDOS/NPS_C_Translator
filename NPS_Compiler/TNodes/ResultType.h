//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_VARIABLETYPE_H
#define NPS_C_TRANSLATOR_VARIABLETYPE_H

#include "../../NPS_library/heap/heap.h"
#include "../../NPS_library/utils/string_utils.h"

class ResultType
{
    char *base_type;
    int p_count;
    bool is_const;
public:
    ResultType(const char *_type, int _p_count, bool _is_const)
    {
        base_type = copy_string(_type);
        p_count = _p_count;
        is_const = _is_const;
    }
    ResultType(const char *_type, bool _is_const) : ResultType(_type, 0, _is_const){}
    ResultType(const char *_type) : ResultType(_type, true){}
    ~ResultType()
    {
        Heap::free_mem(base_type);
    }
    const char* GetBaseType(){return base_type;}
    int GetPCount(){return p_count;}
    bool isConst(){ return is_const;}
    bool operator==(const ResultType &t)
    {return p_count == t.p_count && compare_strings(base_type, t.base_type);}
    bool operator!=(const ResultType &t)
    {return !operator==(t);}
};

#endif //NPS_C_TRANSLATOR_VARIABLETYPE_H
