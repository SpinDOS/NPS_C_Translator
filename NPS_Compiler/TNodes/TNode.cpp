//
// Created by Alexander on 27-Mar-17.
//

#include "TNode.h"
#include "cstring"
#include "../Operations/PrimitiveOperationsManager.h"
#include "../Operations/CustomOperationsManager.h"

ResultType* TBranch::_getType()
{
    ResultType *result = PrimitiveOperationsManager::GetResultOfOperation(this);
    if (result)
        return result;
    return CustomOperationsManager::GetResultOfOperation(this);
}



