#include <iostream>
#include "VariableTable.h"

using namespace NPS_Interpreter;

VisibilityArea* VariableTable::current = new VisibilityArea;

char* VariableTable::GetVariableType(char *key) {
    VisibilityArea *temp = current;
    do
    {
        char* result = temp->table->get(key);
        if(result != nullptr) {
            return result;
        }
        temp = temp->parent;
    }while (temp != nullptr);
    return nullptr;
}

void VariableTable::AddVariable(const char *key, int size)
{
    char* mem = (char*)Heap::get_mem(size);
    current->table->put(key, mem);
}

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
    delete area;
}