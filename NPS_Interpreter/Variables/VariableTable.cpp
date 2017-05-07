#include <iostream>
#include "VariableTable.h"

using namespace NPS_Interpreter;

VisibilityArea* VariableTable::current = new VisibilityArea;

char* VariableTable::GetVariableData(const char *key) {
    VisibilityArea *temp = current;
    do
    {
        char* result = temp->table.get(key);
        if(result != nullptr) {
            return result;
        }
        temp = temp->parent;
    }while (temp != nullptr);
    return nullptr;
}

char* VariableTable::AddVariable(const char *key, int size)
{
    char* mem = Heap::get_mem(size);
    memset(mem, 0, size);
    current->table.put(key, mem);
    current->values.add(mem);
    return mem;
}

char* VariableTable::AddVariableWithData(const char *key, char *data)
{
    current->table.put(key, data);
    current->values.add(data);
    return data;
}

void VariableTable::RemoveVariable(const char *key)
{
    char *mem = current->table.remove(key);
    for (int i = current->values.count() - 1; i >= 0; i--)
        if (current->values.get(i) == mem)
        {
            current->values.take(i);
            break;
        }
    Heap::free_mem(mem);
}

void VariableTable::AddDataToFreeOnPop(char *data)
{current->values.add(data);}

void VariableTable::PushVisibilityArea()
{
    VisibilityArea* area = new VisibilityArea;
    area->parent = current;
    current = area;
}

void VariableTable::PopVisibilityArea()
{
    VisibilityArea* area = current;
    current = current->parent;
    while (area->values.count() > 0)
        Heap::free_mem(area->values.takeFirst());
    delete area;
}