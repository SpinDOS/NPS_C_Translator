

#ifndef NPS_C_TRANSLATOR_VARIABLETABLE_H
#define NPS_C_TRANSLATOR_VARIABLETABLE_H

#include "../../NPS_library/collection_containers/THashTable.h"
#include "../../NPS_library/collection_containers/TSimpleLinkedList.h"

namespace NPS_Interpreter
{
    struct VisibilityArea
    {
        VisibilityArea() { table = new THashTable<char>(5, 5, 5);}
        VisibilityArea* parent = nullptr;
        THashTable<char>* table;
    };

    class VariableTable
    {
        static VisibilityArea *current;
    public:
        static char* GetVariableType(char *key);
        static void AddVariable(const char *key, int size);
        static void PushVisibilityArea();
        static void PopVisibilityArea();
    };
}

#endif //NPS_C_TRANSLATOR_VARIABLETABLE_H
