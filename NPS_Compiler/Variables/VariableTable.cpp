#include "VariableTable.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../ErrorReporter/ErrorReporter.h"

THashTable <TSimpleLinkedList<Func>> functions(3, 3, 3);
VisibilityArea globalArea;
VisibilityArea* VariableTable::current = &globalArea;

ResultType* VariableTable::GetVariableType(LexemeWord *var)
{
    VisibilityArea *cur = current;
    do
    {
        ResultType *result = current->table.get(var->lexeme);
        if(result != nullptr)
            return result;
        cur = cur->parent;
    }
    while (cur != nullptr);
    if (functions.get(var->lexeme) == nullptr)
        ReportError(var, "Variable is not declared");
    return nullptr;
}

void VariableTable::AddVariable(LexemeWord *var, ResultType *type)
{
    if (current != &globalArea)
    {
        if (current->table.get(var->lexeme) != nullptr)
            ReportError(var, "This name for variable is already used");
        else
            current->table.put(var->lexeme, type);
        return;
    }

    // special handling for global area to detect duplicate members

    // search for the same named variables
    bool exists = false;
    TSimpleLinkedList<Func> *overloads =  functions.get(var->lexeme);
    if (overloads == nullptr)
    {
        if (globalArea.table.get(var->lexeme) == nullptr) // if unique var
        {
            exists = false;
            // if type is function and no overloads
            if (type->p_count == 0 && type->baseType->typeOfType == PrimCustFunc::Function)
            {
                TSimpleLinkedList<Func> *list = new TSimpleLinkedList<Func>;
                list->add(static_cast<Func*>(type->baseType));
                functions.put(var->lexeme, list);
            }
            else
                globalArea.table.put(var->lexeme, type);
        }
        else // exists variable
            exists = true;
    }
    else
    {
        // if new var is variable but function with the same name exists
        if (type->p_count > 0 || type->baseType->typeOfType != PrimCustFunc::Function)
            exists = true;
        else // function with the same name exists
        {
            Func *foo = static_cast<Func*>(type->baseType);
            for (int i = 0; i < overloads->count(); i++) // loop for all overload
            {
                Func *overload = overloads->get(i);
                if (overload->parameters.count() != foo->parameters.count())
                    continue;
                exists = true;
                for (int j = 0; exists && j < overload->parameters.count(); j++) // loop for parameters
                    if (*overload->parameters.get(j) != *foo->parameters.get(j))
                        exists = false;
                if (exists)
                {
                    ReportError(var, "This overload of the function is already used");
                    return;
                }
            }
            overloads->add(foo);
        }
    }
    if (exists)
    {
        ReportError(var, "This identifier is already used");
        return;
    }
}

TSimpleLinkedList<Func>* VariableTable::GetOverloads(char *function)
{return functions.get(function);}

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
    Func *func;
    PrimitiveType *primitiveType;
    TSimpleLinkedList<Func> *list;
    ResultType *resultType;

    func = new Func;
    func->returnValue = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("char");
    func->returnValue->baseType = primitiveType;
    func->returnValue->p_count = 1;
    list = functions.get("input");
    if (list == nullptr)
    {
        list = new TSimpleLinkedList<Func>;
        functions.put("input", list);
    }
    list->add(func);


    func = new Func;
    func->returnValue = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("void");
    func->returnValue->baseType = primitiveType;
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("char");
    resultType->baseType = primitiveType;
    resultType->p_count = 1;
    func->parameters.add(resultType);
    list = functions.get("output");
    if (list == nullptr)
    {
        list = new TSimpleLinkedList<Func>;
        functions.put("output", list);
    }
    list->add(func);


    func = new Func;
    func->returnValue = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("int");
    func->returnValue->baseType = primitiveType;
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("int");
    resultType->baseType = primitiveType;
    func->parameters.add(resultType);
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("int");
    resultType->baseType = primitiveType;
    func->parameters.add(resultType);
    list = functions.get("min");
    if (list == nullptr)
    {
        list = new TSimpleLinkedList<Func>;
        functions.put("min", list);
    }
    list->add(func);

    func = new Func;
    func->returnValue = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("int");
    func->returnValue->baseType = primitiveType;
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("int");
    resultType->baseType = primitiveType;
    func->parameters.add(resultType);
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("int");
    resultType->baseType = primitiveType;
    func->parameters.add(resultType);
    list = functions.get("max");
    if (list == nullptr)
    {
        list = new TSimpleLinkedList<Func>;
        functions.put("max", list);
    }
    list->add(func);


    func = new Func;
    func->returnValue = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("double");
    func->returnValue->baseType = primitiveType;
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("double");
    resultType->baseType = primitiveType;
    func->parameters.add(resultType);
    list = functions.get("sin");
    if (list == nullptr)
    {
        list = new TSimpleLinkedList<Func>;
        functions.put("sin", list);
    }
    list->add(func);

    func = new Func;
    func->returnValue = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("double");
    func->returnValue->baseType = primitiveType;
    resultType = new ResultType;
    primitiveType = new PrimitiveType;
    primitiveType->type = copy_string("double");
    resultType->baseType = primitiveType;
    func->parameters.add(resultType);
    list = functions.get("cos");
    if (list == nullptr)
    {
        list = new TSimpleLinkedList<Func>;
        functions.put("cos", list);
    }
    list->add(func);
}