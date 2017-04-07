//
// Created by Alexander on 27-Mar-17.
//

#include "TNode.h"
#include "cstring"
#include "../Operations/PrimitiveOperationsManager.h"
#include "../Operations/CustomOperationsManager.h"

ResultType* TOperation::_getType()
{
    ResultType *result = PrimitiveOperationsManager::GetResultOfOperation(this);
    if (result)
        return result;
    return CustomOperationsManager::GetResultOfOperation(this);
}

ResultType* TList::_getType()
{
    for (int i = 0; i < children.count(); i++)
        children.get(i)->getType();
    return nullptr;
}



