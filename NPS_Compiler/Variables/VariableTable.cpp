#include "VariableTable.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../ErrorReporter/ErrorReporter.h"

VisibilityArea* VariableTable::current = nullptr;

ResultType* VariableTable::GetVariableType(LexemeWord *var) {
    ResultType *result;
    VisibilityArea *temp = current;
    do
    {
        result = current->table.get(var->lexeme);
        if(result != nullptr)
            return result;
        temp = temp->parent;
    }while (temp != nullptr);
    ReportError(var, "not found");
    return nullptr;
}

void VariableTable::AddVariable(const char *var, ResultType *type)
{
    current->table.put(var, type);
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