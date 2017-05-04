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
    if (arrayLength != nullptr)
        printf("Arrays are not supported yet");
    VariableTable::AddVariable(name, size);
    return ReturnResult(VariableTable::GetVariableData(name), false);
}

ReturnResult TFunctionParamsGetter::Exec()
{
    VariableTable::AddVariable(name, size);
    void *in_table = VariableTable::GetVariableData(name);
    ReturnResult actual = GlobalParameters()->take_first();
    memcpy(in_table, actual.data, size);
    if (actual.need_to_free_mem)
        Heap::free_mem(actual.data);
    return ReturnResult();
}

ReturnResult TList::Exec()
{
    VariableTable::PushVisibilityArea();
    for (int i = 0; i < children.count(); i++)
    {
        void *temp = VariableTable::GetVariableData("break");
        if (temp && *static_cast<bool*>(temp))
            break;
        temp = VariableTable::GetVariableData("continue");
        if (temp && *static_cast<bool*>(temp))
            break;

        temp = VariableTable::GetVariableData("return");
        if (temp && *static_cast<bool*>(temp))
            break;
        TNode *node = children.get(i);
        ReturnResult result = node->Exec();
        if (result.need_to_free_mem)
            Heap::free_mem(result.data);
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
    *reinterpret_cast<bool*>(VariableTable::GetVariableData("return")) = false;
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