#include "TNode.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;

TypeList<ReturnResult> *_globalParameters = new TypeList<ReturnResult>;
TypeList<ReturnResult> *NPS_Interpreter::GlobalParameters() {return _globalParameters;}

ReturnResult TConstant::Exec()
{ return ReturnResult(this->data, false);}

ReturnResult TVariable::Exec()
{ return ReturnResult(VariableTable::GetVariableData(name), false); }

ReturnResult TDeclaration::Exec()
{
    VariableTable::AddVariable(name, size);
    char *var = VariableTable::GetVariableData(name);
    if (arrayLength != nullptr)
    {
        int length = *reinterpret_cast<int*>(arrayLength->Exec().data);
        char *arrayData = (char*)Heap::get_mem(length * underlying_size);
        memset(arrayData, 0, length * underlying_size);
        VariableTable::AddDataToFreeOnPop(arrayData);
        *reinterpret_cast<char**>(var) = arrayData;
    }
    return ReturnResult(var, false);
}

ReturnResult TFunctionParamsGetter::Exec()
{
    VariableTable::AddVariable(name, size);
    void *in_table = VariableTable::GetVariableData(name);
    ReturnResult actual = GlobalParameters()->take_first();
    memcpy(in_table, actual.data, size);
    actual.FreeIfNeed();
    return ReturnResult();
}

ReturnResult TList::Exec()
{
    VariableTable::PushVisibilityArea();
    for (int i = 0; i < children.count(); i++)
    {
        children.get(i)->Exec().FreeIfNeed();

        if (*reinterpret_cast<bool*>(VariableTable::GetVariableData("continue")) ||
            *reinterpret_cast<bool*>(VariableTable::GetVariableData("break")) ||
            *reinterpret_cast<bool*>(VariableTable::GetVariableData("return")))
            break;

    }
    VariableTable::PopVisibilityArea();
    return ReturnResult();
}

ReturnResult TFunction::Exec()
{
    TList *function = *reinterpret_cast<TList**>(children.getFirst()->Exec().data);
    for (int i = 1; i < this->children.count(); i++)
        GlobalParameters()->addTyped(this->children.get(i)->Exec());
    VariableTable::AddVariable("return", sizeof(bool));
    function->Exec();
    VariableTable::RemoveVariable("return");
    if (GlobalParameters()->count() > 0)
        return GlobalParameters()->take_first();
    return ReturnResult();
}

ReturnResult TFunctionDefinition::Exec()
{
    VariableTable::AddVariable(name, sizeof(TList*));
    TList *list = new TList;
    memcpy(VariableTable::GetVariableData(name), &list, sizeof(TList*));
    for(int i = 0; i < children.count(); i++)
        list->children.add(children.get(i));
    return ReturnResult();
}