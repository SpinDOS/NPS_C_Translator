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
    static ResultType *nps_increment(TOperation *operation);
    static ResultType *nps_decrement(TOperation *operation);
    static ResultType *nps_uMinus(TOperation *operation);
    static ResultType *nps_uPlus(TOperation *operation);
    static ResultType *nps_bMinus(TOperation *operation);
    static ResultType *nps_bPlus(TOperation *operation);
    static ResultType *nps_logicComplement(TOperation *operation); // !
    static ResultType *nps_bitwiseComplement(TOperation *operation); // ~
    static ResultType *nps_multiplication(TOperation *operation);
    static ResultType *nps_division(TOperation *operation);
    static ResultType *nps_MOD(TOperation *operation); // %
    static ResultType *nps_leftShift(TOperation *operation); // <<
    static ResultType *nps_rightShift(TOperation *operation); // >>
    static ResultType *nps_less(TOperation *operation); // <
    static ResultType *nps_great(TOperation *operation); // >
    static ResultType *nps_lessEqual(TOperation *operation); // <=
    static ResultType *nps_greatEqual(TOperation *operation); // >=
    static ResultType *nps_equal(TOperation *operation); // ==
    static ResultType *nps_notEqual(TOperation *operation); // !=
    static ResultType *nps_bitwiseAND(TOperation *operation); // & - bitAND
    static ResultType *nps_bitwiseXOR(TOperation *operation); // ^
    static ResultType *nps_bitwiseOR(TOperation *operation); // |
    static ResultType *nps_logicAND(TOperation *operation); // &&
    static ResultType *nps_logicOR(TOperation *operation); // ||
public:
    static void Init();
    static ResultType *GetResultOfOperation(TOperation *operation);
};

#endif //NPS_C_TRANSLATOR_PRIMITIVEOPERATIONSMANAGER_H