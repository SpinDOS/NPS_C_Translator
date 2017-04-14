#include "VariableTable.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../ErrorReporter/ErrorReporter.h"

VisibilityArea* current;

ResultType* VariableTable::GetVariableType(const char *var) {
    ResultType *result;
    VisibilityArea *temp = current;
    do
    {
        result = current->table.get((char *) var);
        if(result != nullptr)
            return result;
        temp = temp->parent;
    }while (temp != nullptr);
    ReportError("error", "not found");
    return nullptr;
}

void VariableTable::AddVariable(const char *var, ResultType *type)
{
    current->table.put((char *) var, type);
}

void VariableTable::PushVisibilityArea()
{
    VisibilityArea* area = (VisibilityArea*)Heap::get_mem(sizeof(VisibilityArea));
    area->parent = current;
    current = area;
}

void VariableTable::PopVisibilityArea()
{
    VisibilityArea* area = current;
    current = current->parent;
    Heap::free_mem(area);
}