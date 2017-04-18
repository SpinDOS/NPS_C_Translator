//
// Created by Alexander on 01-Apr-17.
//

#include "FunctionsManager.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../ErrorReporter/ErrorReporter.h"

using namespace NPS_Compiler;

TSimpleLinkedList<FunctionDefinition> functions;

void FunctionsManager::AddFunction(FunctionDefinition *function)
{
    // control duplicating of system functions

    bool duplicate = false;
    char *name = function->name->lexeme;

    if (strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0)
    {
        ResultType doubleResult("double");
        duplicate = function->paramTypes.count() == 1 &&
            *function->paramTypes.getFirst() == doubleResult;
    }
    else if (strcmp(name, "min") || strcmp(name, "max") == 0)
    {
        ResultType intResult("int");
        duplicate = function->paramTypes.count() == 2 &&
                    *function->paramTypes.getFirst() == intResult &&
                    *function->paramTypes.getLast() == intResult;
    }
    else if (strcmp(name, "input") == 0)
    {
        duplicate = function->paramTypes.count() == 0;
    }
    else if (strcmp(name, "output") == 0)
    {
        ResultType strResult("char", 1);
        duplicate = function->paramTypes.count() == 1 &&
                    *function->paramTypes.getFirst() == strResult;
    }
    if (duplicate)
    {
        ReportError(function->name, "You can not declare system function");
        return;
    }

    // control duplicating of user functions
    for (int i = 0; i < functions.count(); i++)
    {
        FunctionDefinition *overload = functions.get(i);
        if (*overload == *function)
        {
            ReportError(function->name, "This overload of the function is already used");
            return;
        }
    }
    functions.add(function);
}

bool FunctionsManager::IsFunctionExists(const char *name)
{
    if (strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 ||
            strcmp(name, "min") == 0 || strcmp(name, "max") == 0 ||
            strcmp(name, "input") == 0 || strcmp(name, "output") == 0)
        return true;
    for (int i = 0; i < functions.count(); i++)
        if (strcmp(functions.get(i)->name->lexeme, name) == 0)
            return true;
    return false;
}