#include "VariableTable.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../TypeCast/TypeCastManager.h"
#include "../Operations/OperationsManager.h"

using namespace NPS_Compiler;

VisibilityArea globalArea;
VisibilityArea* VariableTable::current = &globalArea;

ResultType* VariableTable::GetVariableType(const char *var)
{
    VisibilityArea *cur = current;
    do
    {
        ResultType *result = cur->table.get(var);
        if(result != nullptr)
            return result;
        cur = cur->parent;
    }
    while (cur != nullptr);
    return nullptr;
}

void VariableTable::AddVariable(LexemeWord *var, ResultType *type)
{
    if (current->table.get(var->lexeme) != nullptr)
        ReportError(var, "This name for variable is already used");
    else
    {
        string extended = string(var->lexeme) + "#0";
        if (globalArea.table.get(extended.c_str()) != nullptr)
            ReportError(var, "Function with the same name exists");
        else
            current->table.put(var->lexeme, type);
    }
}

void VariableTable::InitializeGlobal(TSimpleLinkedList<NPS_Compiler::TNode> *globalDefinitions)
{
    for (int i = 0; i < globalDefinitions->count(); i++)
    {
        if (globalDefinitions->get(i)->tNodeType != TNodeTypeFunctionDefinition)
            continue;
        TFunctionDefinition *definition = static_cast<TFunctionDefinition*>
                                            (globalDefinitions->get(i));
        
        // handle special functions
        if (200 <= definition->lexeme->code && definition->lexeme->code < 300 &&
                !OperationsManager::ValidateCustomOperator(definition->signature, definition->lexeme))
            return;
        
        bool isCast = false;
        if (strcmp(definition->lexeme->lexeme, "implicit") == 0 ||
                strcmp(definition->lexeme->lexeme, "explicit") == 0)
        {
            isCast = true;
            if (!TypeCastManager::ValidateCustomCast(definition->signature,
                                                     definition->lexeme))
                return;
        }
        // operators here
        int j;
        string name;
        for (j = 0; ; j++)
        {
            name = string(definition->lexeme->lexeme) + "#" + to_string(j);
            ResultType *existing = globalArea.table.get(name.c_str());
            if (existing == nullptr)
                break;
            Func *existingSignature = static_cast<Func*>(existing->baseType);
            bool same = existingSignature->parameters.count() == definition->signature->parameters.count();
            for (int k = 0; same && k < existingSignature->parameters.count(); k++)
                if (*existingSignature->parameters.get(k) != *definition->signature->parameters.get(k))
                    same = false;
            // overload cast operator by return value
            if (same && isCast && *definition->signature->returnValue != *existingSignature->returnValue)
                same = false;
            if (same)
            {
                ReportError(definition->lexeme, "Function overload with the same signature exists");
                return;
            }
        }
        // add to variable table
        ResultType *resultType = new ResultType;
        resultType->baseType = definition->signature;
        globalArea.table.put(name.c_str(), resultType);
        Heap::free_mem(definition->lexeme->lexeme);
        definition->lexeme->lexeme = copy_string(name.c_str());
    }
    
    // check main exists and single
    ResultType *main0 = globalArea.table.get("main#0");
    if (main0 == nullptr || main0->p_count != 0|| main0->baseType->typeOfType != PrimCustFunc::Function)
        ReportError(0l, "Can not find main function");
    else if (globalArea.table.get("main#1") != nullptr)
        ReportError(0l, "There are multiple main functions");
    else
    {
        Func *mainSignature = static_cast<Func*>(main0->baseType);
        if (mainSignature->returnValue == TypesManager::Int())
        {
            if (mainSignature->parameters.count() == 0)
                return;
            ResultType *char2p = TypesManager::Char()->clone();
            char2p->p_count = 2;
            if (mainSignature->parameters.count() == 2 &&
                mainSignature->parameters.getFirst() == TypesManager::Int() &&
                mainSignature->parameters.getLast() == char2p)
            {
                delete char2p;
                return;
            }
        }
        ReportError(0l, "Invalid main function signature");
    }
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

void VariableTable::Init()
{
    ResultType *resultType;
    Func *func;
    ResultType *charPointer = TypesManager::Char()->clone();
    charPointer->p_count = 1;

    resultType = new ResultType;
    func = new Func;
    func->returnValue = charPointer;
    resultType->baseType = func;
    globalArea.table.put("input#0", resultType);
    
    
    resultType = new ResultType;
    func = new Func;
    func->returnValue = TypesManager::Void();
    func->parameters.add(charPointer);
    resultType->baseType = func;
    globalArea.table.put("output#0", resultType);
    
    
    resultType = new ResultType;
    func = new Func;
    func->returnValue = TypesManager::Int();
    func->parameters.add(TypesManager::Int());
    func->parameters.add(TypesManager::Int());
    resultType->baseType = func;
    globalArea.table.put("min#0", resultType);
    globalArea.table.put("max#0", resultType);
    
    
    resultType = new ResultType;
    func = new Func;
    func->returnValue = TypesManager::Double();
    func->parameters.add(TypesManager::Double());
    resultType->baseType = func;
    globalArea.table.put("sin#0", resultType);
    globalArea.table.put("cos#0", resultType);
}