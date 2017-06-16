//
// Created by Alexander on 02-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_TYPEINFO_H
#define NPS_C_TRANSLATOR_TYPEINFO_H

#include "../../NPS_library/utils/string_utils.h"
#include "../../NPS_library/heap/heap.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../TNodes/ResultType.h"

struct FieldInfo
{
    char *name = nullptr;
    int offset = -1;
    ResultType *type = nullptr;
};

class TypeInfo
{
public:
    TypeInfo(const char *type) : fields(2, 2) {typeName = copy_string(type);}
    ~TypeInfo(){Heap::free_mem(typeName);}
    char *typeName;
    int size = -1;
    THashTable<FieldInfo> fields;
};

#endif //NPS_C_TRANSLATOR_TYPEINFO_H
