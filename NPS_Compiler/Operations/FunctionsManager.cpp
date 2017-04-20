//
// Created by Alexander on 01-Apr-17.
//

#include "FunctionsManager.h"
#include "../../NPS_library/collection_containers/THashTable.h"
#include "../ErrorReporter/ErrorReporter.h"

using namespace NPS_Compiler;

TSimpleLinkedList<KeyValuePair<char*, Func>> functions;

void FunctionsManager::AddFunction(KeyValuePair<char*, Func> *function)
{
    // control duplicating of system functions

    bool duplicate = false;
    char *name = function->key;

    if (strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0)
    {
        ResultType doubleResult;
        doubleResult.baseType = "double";
        duplicate = function->paramTypes.count() == 1 &&
            *function->paramTypes.getFirst() == doubleResult;
    }
    else if (strcmp(name, "min") || strcmp(name, "max") == 0)
    {
        ResultType doubleResult;
        doubleResult.baseType = "int";
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
        ResultType doubleResult;
        doubleResult.baseType = "char";
        doubleResult.p_count = 1;
        duplicate = function->paramTypes.count() == 1 &&
                    *function->paramTypes.getFirst() == strResult;
    }

    if (duplicate)
    {
        ReportError(function->name, "You can not redefine system function");
        return;
    }

    TSimpleLinkedList<ResultType> *parameters = &function->value->parameters;
    // control duplicating of user functions
    for (int i = 0; i < functions.count(); i++)
    {
        KeyValuePair<char*, Func> *overload = functions.get(i);
        duplicate = strcmp(*overload->key, *function->key) == 0;
        TSimpleLinkedList<ResultType> *parameters1 = &overload->value->parameters;
        if (parameters->count() != parameters1->count())
            continue;
        for (int i = 0; !duplicate && i < parameters->count(); i++)
            if (*parameters->get(i) == *parameters1->get(i))
                duplicate = true;

        if (duplicate)
        {
            ReportError(function->name, "This overload of the function is already used");
            return;
        }
    }
    functions.add(function);
}
