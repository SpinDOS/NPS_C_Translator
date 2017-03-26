//
// Created by Alexander on 27-Mar-17.
//

#include "TNode.h"
#include "cstring"

TNode* GetTNode(LexemeWord *lexeme)
{
    TOperation *op = new TOperation;
    TConstant *con = new TConstant;
    con->data = Heap::get_mem(sizeof(double));
    NumConstantType type;
    double d;
    switch (lexeme->code)
    {
        case 221:
            op->type_of_operation = 0;
            op->priority = 1;
            return op;
        case 222:
            op->type_of_operation = 1;
            op->priority = 1;
            return op;
        case 218:
            op->type_of_operation = 2;
            op->priority = 2;
            return op;
        case 219:
            op->type_of_operation = 3;
            op->priority = 2;
            return op;
        default:
            d = parse_num_constant(*lexeme, type);
            memcpy(con->data, &d, sizeof(double));
            return con;
    }
}


void* TOperation::Exec()
{
    TNode * t = (*children.getTyped(1));
    double *i1 = (double *)(*children.getTyped(0))->Exec();
    double *i2 = (double *)(*children.getTyped(1))->Exec();
    double *res = (double *) Heap::get_mem(sizeof(double));
    switch (type_of_operation)
    {
        case 0:
            *res = *i1 + *i2;
            break;
        case 1:
            *res = *i1 - *i2;
            break;
        case 2:
            *res = *i1 * *i2;
            break;
        case 3:
            *res = *i1 / *i2;
            break;
    }
    return res;
}