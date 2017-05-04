
#include "TNode.h"
#include "../Variables/VariableTable.h"
#include "../Functions/ParameterManager.h"
#include "../../NPS_Compiler/TNodes/ResultType.h"

using namespace NPS_Interpreter;

char* TDeclaration::Exec() {
    VariableTable::AddVariable(key, size);
    return VariableTable::GetVariableData(key);
}

char* TVariable::Exec(){
    return VariableTable::GetVariableData(key);
}

char* TOperation::Exec()
{
    char *first = this->children.takeFirst()->Exec();
    char *second = this->children.count() >= 2? this->children.get(1)->Exec() : nullptr;
    char *third = this->children.count() == 3? this->children.getLast()->Exec() : nullptr;
    return this->handler(first, second, third, this->size);
}

char* TFunction::Exec()
{
    char *function = (children.getFirst())->Exec();
    FuncContainer *func = (FuncContainer *) function;
    for (int i = 1; i < this->children.count(); i++)
        ParametersManager::global_parameters.add(this->children.get(i)->Exec());
    VariableTable::AddVariable("return", sizeof(bool));
    *static_cast<bool*>(VariableTable::GetVariableData("return")) = false;
    func->instructions->Exec();
    VariableTable::RemoveVariable("return");
    if (ParametersManager::global_parameters.count() > 0)
        return ParametersManager::global_parameters.takeFirst();
    return nullptr;
}

char* TList::Exec() {
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
        children.get(i)->Exec();
    }
    VariableTable::PopVisibilityArea();
    return nullptr;
}

char* TFunctionDefinition::Exec(){
    VariableTable::AddVariable(name, sizeof(FuncContainer));
    FuncContainer* func = static_cast<FuncContainer*>(VariableTable::GetVariableData(name));
    TList *list = func->instructions = new TList;
    for(int i = 0; i < children.count(); i++){
        list->children.add(children.get(i));
    }
}

char* TFunctionParamsGetter::Exec()
{
    void *data = TDeclaration::Exec();
    void *param = ParametersManager::global_parameters.takeFirst();
    memcpy(data, param, size);
    return nullptr;
}





