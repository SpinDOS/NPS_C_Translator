//
// Created by Alexander on 02-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_TYPEINFO_H
#define NPS_C_TRANSLATOR_TYPEINFO_H

#include "../../NPS_library/utils/string_utils.h"
#include "../../NPS_library/heap/heap.h"

class TypeInfo
{
    char *_type;
public:
    TypeInfo(const char *type){_type = copy_string(type);}
    ~TypeInfo(){Heap::free_mem(_type);}
    const char *type(){return _type;}
    int size;
    //Hashtable<char*, TypeInfo*> fields;
    //int GetFieldOffset(const char *fieldName);
    //TNode *GetFieldOffset(const char *functionName);
};

#endif //NPS_C_TRANSLATOR_TYPEINFO_H
