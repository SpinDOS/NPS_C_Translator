//
// Created by Alexander on 01-Apr-17.
//
#ifndef NPS_C_TRANSLATOR_PRIMITIVEOPERATIONSMANAGER_H
#define NPS_C_TRANSLATOR_PRIMITIVEOPERATIONSMANAGER_H

#include "../TNodes/ResultType.h"
#include "../TNodes/TNode.h"

using namespace NPS_Compiler;

class PrimitiveOperationsManager
{
private:
    static const ResultType NPS_BOOL;
    static const ResultType NPS_CHAR;
    static const ResultType NPS_DOUBLE;
    static const ResultType NPS_INT;
    static ResultType *nps_increment(TBranch *operation);
    static ResultType *nps_decrement(TBranch *operation);
    static ResultType *nps_uMinus(TBranch *operation);
    static ResultType *nps_uPlus(TBranch *operation);
    static ResultType *nps_bMinus(TBranch *operation);
    static ResultType *nps_bPlus(TBranch *operation);
    static ResultType *nps_logicComplement(TBranch *operation); // !
    static ResultType *nps_bitwiseComplement(TBranch *operation); // ~
    static ResultType *nps_indirection(TBranch *operation); // *
    static ResultType *nps_reference(TBranch *operation); // & - ref
    static ResultType *nps_multiplication(TBranch *operation);
    static ResultType *nps_division(TBranch *operation);
    static ResultType *nps_MOD(TBranch *operation); // %
    static ResultType *nps_leftShift(TBranch *operation); // <<
    static ResultType *nps_rightShift(TBranch *operation); // >>
    static ResultType *nps_less(TBranch *operation); // <
    static ResultType *nps_great(TBranch *operation); // >
    static ResultType *nps_relationEqual(TBranch *operation); // ==
    static ResultType *nps_notEqual(TBranch *operation); // !=
    static ResultType *nps_equal(TBranch *operation); // =
    static ResultType *nps_bitwiseAND(TBranch *operation); // & - bitAND
    static ResultType *nps_bitwiseXOR(TBranch *operation); // ^
    static ResultType *nps_bitwiseOR(TBranch *operation); // |
    static ResultType *nps_logicAND(TBranch *operation); // &&
    static ResultType *nps_logicOR(TBranch *operation); // ||
    static ResultType *nps_ternaryOperator(TBranch *operation); // ?:
    static ResultType *nps_comma(TBranch *operation); // ,
public:
    static ResultType *GetResultOfOperation(TBranch *operation);
};

#endif //NPS_C_TRANSLATOR_PRIMITIVEOPERATIONSMANAGER_H