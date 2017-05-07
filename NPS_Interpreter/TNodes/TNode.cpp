#include "TNode.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;

TSimpleLinkedList<char> *_globalParameters = new TSimpleLinkedList<char>;
TSimpleLinkedList<char> *NPS_Interpreter::GlobalParameters() {return _globalParameters;}

char* TConstant::Exec()
{ return this->data;}

char* TVariable::Exec()
{ return VariableTable::GetVariableData(name); }

char* TDeclaration::Exec()
{
    if (arrayLength == nullptr)
        return VariableTable::AddVariable(name, size);

    int *length_data = reinterpret_cast<int*>(arrayLength->Exec());
    int length = *length_data;
    arrayLength->free_my_mem(length_data);
    char *arrayData = (char*)Heap::get_mem(length * underlying_size);
    memset(arrayData, 0, length * underlying_size);
    VariableTable::AddDataToFreeOnPop(arrayData);
    return VariableTable::AddVariableWithData(name, reinterpret_cast<char*>(&arrayData));
}

char* TFunctionParamsGetter::Exec()
{
    return VariableTable::AddVariableWithData(name, GlobalParameters()->takeFirst());
}

char* TList::Exec()
{
    VariableTable::PushVisibilityArea();
    for (int i = 0; i < children.count(); i++)
    {
        this->children.get(i)->ExecAndFree();

        if (*reinterpret_cast<bool*>(VariableTable::GetVariableData("continue")) ||
            *reinterpret_cast<bool*>(VariableTable::GetVariableData("break")) ||
            *reinterpret_cast<bool*>(VariableTable::GetVariableData("return")))
            break;
    }
    VariableTable::PopVisibilityArea();
    return nullptr;
}

char* TFunction::Exec()
{
    for (int i = 1; i < this->children.count(); i++)
    {
        TNode *node = this->children.get(i);
        char *data = node->Exec();
        int size = *this->params_sizes.getTyped(i -1);
        char *param = (char*) Heap::get_mem(size);
        memcpy(param, data, size);
        GlobalParameters()->add(param);
        node->free_my_mem(data);
    }
    char *function_name = this->children.getFirst()->Exec();
    TList *function = reinterpret_cast<TList*>(function_name);
    VariableTable::AddVariable("return", sizeof(bool));
    function->Exec();
    VariableTable::RemoveVariable("return");
    this->children.getFirst()->free_my_mem(function_name);
    
    this->need_to_free_my_mem = GlobalParameters()->count() > 0;
    if (this->need_to_free_my_mem)
        return GlobalParameters()->takeFirst();
    return nullptr;
}

char* TFunctionDefinition::Exec()
{
    TList *list = new TList;
    VariableTable::AddVariableWithData(name, reinterpret_cast<char*>(list));
    for (int i = 0; i < children.count(); i++)
        list->children.add(children.get(i));
    return nullptr;
}