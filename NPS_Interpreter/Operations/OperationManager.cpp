//
// Created by Alexander on 04-May-17.
//

#include "iostream"
#include "OperationManager.h"
#include "../../NPS_library/utils/string_utils.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;
using namespace std;

// =======================================================================
// helpers

void* get_pointer (TNode *node)
{
    char *result = node->Exec();
    void *data = *reinterpret_cast<char**>(result);
    node->free_my_mem(result);
    return data;
}

char get_bool(TNode *node)
{
    char *result = node->Exec();
    bool data = *reinterpret_cast<bool*>(result);
    node->free_my_mem(result);
    return data;
}

char get_char(TNode *node)
{
    char *result = node->Exec();
    char data = *result;
    node->free_my_mem(result);
    return data;
}

int get_int(TNode *node)
{
    char *result = node->Exec();
    int data = *reinterpret_cast<int*>(result);
    node->free_my_mem(result);
    return data;
}

double get_double(TNode *node)
{
    char *result = node->Exec();
    double data = *reinterpret_cast<double*>(result);
    node->free_my_mem(result);
    return data;
}

char* res(void *p)
{
    char *result = Heap::get_mem(sizeof(void*));
    *reinterpret_cast<void**>(result) = p;
    return result;
}

char* res(bool b)
{
    char *result = Heap::get_mem(sizeof(bool));
    *reinterpret_cast<bool*>(result) = b;
    return result;
}

char* res(char ch)
{
    char *result = Heap::get_mem(sizeof(char));
    *result = ch;
    return result;
}

char* res(int i)
{
    char *result = Heap::get_mem(sizeof(int));
    *reinterpret_cast<int*>(result) = i;
    return result;
}

char* res(double d)
{
    char *result = Heap::get_mem(sizeof(double));
    *reinterpret_cast<double*>(result) = d;
    return result;
}

// =======================================================================
// casts

struct TOpCastPtoP : TOperation
{ char* Exec() final { return this->children.getFirst()->Exec(); } };


struct TOpCastPtoB : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) != nullptr); } };

struct TOpCastCtoB : TOperation
{ char* Exec() final { return res(get_char(this->children.getFirst()) != '\0'); } };

struct TOpCastItoB : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) != 0); } };

struct TOpCastDtoB : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) != 0.0); } };


struct TOpCastItoC : TOperation
{ char* Exec() final { return res((char)get_int(this->children.getFirst()));} };

struct TOpCastDtoC : TOperation
{ char* Exec() final { return res((char)get_double(this->children.getFirst()));} };


struct TOpCastCtoI : TOperation
{ char* Exec() final { return res((int)get_char(this->children.getFirst()));} };

struct TOpCastDtoI : TOperation
{ char* Exec() final { return res((int)get_double(this->children.getFirst()));} };


struct TOpCastCtoD : TOperation
{ char* Exec() final { return res((double)get_char(this->children.getFirst()));} };

struct TOpCastItoD : TOperation
{ char* Exec() final { return res((double)get_int(this->children.getFirst()));} };

// ==================================================================================
// increment/decrement

char* prefix_int_inc_dec(TOperation *operation, int dif)
{
    char *param = operation->children.getFirst()->Exec();
    *reinterpret_cast<int *>(param) += dif;
    return param;
}

struct TOpPreIncI : TAssignationOperation
{ char* Exec() final { return prefix_int_inc_dec(this, 1); } };

struct TOpPreDecI : TAssignationOperation
{ char* Exec() final { return prefix_int_inc_dec(this, -1); } };

char* postfix_int_inc_dec(TOperation *operation, int dif)
{
    char *param = operation->children.getFirst()->Exec();
    int *param_value = reinterpret_cast<int*>(param);
    char *result = Heap::get_mem(sizeof(int));
    *reinterpret_cast<int*>(result) = *param_value;
    *param_value += dif;
    return result;
}

struct TOpPostIncI : TOperation
{ char* Exec() final { return postfix_int_inc_dec(this, 1); } };

struct TOpPostDecI : TOperation
{ char* Exec() final { return postfix_int_inc_dec(this, -1); } };


char* prefix_double_inc_dec(TOperation *operation, double dif)
{
    char *param = operation->children.getFirst()->Exec();
    *reinterpret_cast<double *>(param) += dif;
    return param;
}

struct TOpPreIncD : TAssignationOperation
{ char* Exec() final { return prefix_double_inc_dec(this, 1.0); } };

struct TOpPreDecD : TAssignationOperation
{ char* Exec() final { return prefix_double_inc_dec(this, -1.0); } };

char* postfix_double_inc_dec(TOperation *operation, double dif)
{
    char *param = operation->children.getFirst()->Exec();
    double *param_value = reinterpret_cast<double*>(param);
    char* result = Heap::get_mem(sizeof(double));
    *reinterpret_cast<double*>(result) = *param_value;
    *param_value += dif;
    return result;
}

struct TOpPostIncD : TOperation
{ char* Exec() final { return postfix_double_inc_dec(this, 1.0); } };

struct TOpPostDecD : TOperation
{ char* Exec() final { return postfix_double_inc_dec(this, -1.0); } };


char* prefix_pointer_inc_dec(TOperation *operation, int dif)
{
    char* param = operation->children.getFirst()->Exec();
    *reinterpret_cast<char**>(param) += dif;
    return param;
}

struct TOpPreIncP : TAssignationOperation
{ char* Exec() final { return prefix_pointer_inc_dec(this, operands_size); } };

struct TOpPreDecP : TAssignationOperation
{ char* Exec() final { return prefix_pointer_inc_dec(this, -operands_size); } };

char* postfix_pointer_inc_dec(TOperation *operation, int dif)
{
    char* param = operation->children.getFirst()->Exec();
    char **param_value = reinterpret_cast<char**>(param);
    char *result = Heap::get_mem(sizeof(void*));
    *reinterpret_cast<char**>(result) = *param_value;
    *param_value += dif;
    return result;
}

struct TOpPostIncP : TOperation
{ char* Exec() final { return postfix_pointer_inc_dec(this, operands_size); } };

struct TOpPostDecP : TOperation
{ char* Exec() final { return postfix_pointer_inc_dec(this, -operands_size); } };

// ==================================================================================
// unary ops

struct TOpUnaryPlusI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst())); } };

struct TOpUnaryPlusD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst())); } };

struct TOpUnaryMinusI : TOperation
{ char* Exec() final { return res(-get_int(this->children.getFirst())); } };

struct TOpUnaryMinusD : TOperation
{ char* Exec() final { return res(-get_double(this->children.getFirst())); } };


struct TOpNotBool : TOperation
{ char* Exec() final { return res(!get_bool(this->children.getFirst())); } };

struct TOpBitwiseNotB : TOperation
{ char* Exec() final { return res(~get_bool(this->children.getFirst())); } };

struct TOpBitwiseNotI : TOperation
{ char* Exec() final { return res(~get_int(this->children.getFirst())); } };

struct TOpDereference : TOperation
{
    TOpDereference(){need_to_free_my_mem = false;}
    char* Exec() final
    {
        char* param = this->children.getFirst()->Exec();
        char *pointer = *reinterpret_cast<char**>(param);
        this->children.getFirst()->free_my_mem(param);
        return pointer;
    }
};

struct TOpReference : TOperation
{
    char* Exec() final
    {
        char *param = this->children.getFirst()->Exec();
        char *pointer = Heap::get_mem(sizeof(void*));
        *reinterpret_cast<char**>(pointer) = param;
        return pointer;
    }
};

// ==================================================================================
// binary operations

struct TOpMulI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) *
                                        get_int(this->children.getLast())); } };

struct TOpMulD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) *
                                         get_double(this->children.getLast())); } };

struct TOpDivI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) /
                                         get_int(this->children.getLast())); } };

struct TOpDivD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) /
                                         get_double(this->children.getLast())); } };

struct TOpModI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) %
                                         get_int(this->children.getLast())); } };


struct TOpBinPlusI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) +
                                         get_int(this->children.getLast())); } };

struct TOpBinPlusD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) +
                                         get_double(this->children.getLast())); } };

struct TOpBinPlusP : TOperation
{ char* Exec() final { return res(( (char*) get_pointer(this->children.getFirst()) ) +
                                         get_int(this->children.getLast()) * this->operands_size); } };

struct TOpBinMinusI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) -
                                         get_int(this->children.getLast())); } };

struct TOpBinMinusD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) -
                                         get_double(this->children.getLast())); } };

struct TOpBinMinusP : TOperation
{ char* Exec() final { return res(( (char*) get_pointer(this->children.getFirst()) ) -
                                         get_int(this->children.getLast()) * this->operands_size); } };

struct TOpShiftRight : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) >>
                                         get_int(this->children.getLast())); } };

struct TOpShiftLeft : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) <<
                                         get_int(this->children.getLast())); } };


struct TOpCmpLessP : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) <
                                         get_pointer(this->children.getLast())); } };

struct TOpCmpLessD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) <
                                         get_double(this->children.getLast())); } };

struct TOpCmpMoreP : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) >
                                         get_pointer(this->children.getLast())); } };

struct TOpCmpMoreD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) >
                                         get_double(this->children.getLast())); } };

struct TOpCmpLessEqualP : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) <=
                                         get_pointer(this->children.getLast())); } };

struct TOpCmpLessEqualD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) <=
                                         get_double(this->children.getLast())); } };

struct TOpCmpMoreEqualP : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) >=
                                         get_pointer(this->children.getLast())); } };

struct TOpCmpMoreEqualD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) >=
                                         get_double(this->children.getLast())); } };

struct TOpCmpEqualP : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) ==
                                         get_pointer(this->children.getLast())); } };

struct TOpCmpEqualB : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) ==
                                         get_bool(this->children.getLast())); } };

struct TOpCmpEqualD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) ==
                                         get_double(this->children.getLast())); } };

struct TOpCmpNotEqualP : TOperation
{ char* Exec() final { return res(get_pointer(this->children.getFirst()) !=
                                         get_pointer(this->children.getLast())); } };

struct TOpCmpNotEqualB : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) !=
                                         get_bool(this->children.getLast())); } };

struct TOpCmpNotEqualD : TOperation
{ char* Exec() final { return res(get_double(this->children.getFirst()) !=
                                         get_double(this->children.getLast())); } };

struct TOpBitwiseAndB : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) &
                                         get_bool(this->children.getLast())); } };

struct TOpBitwiseAndI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) &
                                         get_int(this->children.getLast())); } };

struct TOpBitwiseXorB : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) ^
                                         get_bool(this->children.getLast())); } };

struct TOpBitwiseXorI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) ^
                                         get_int(this->children.getLast())); } };

struct TOpBitwiseOrB : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) |
                                         get_bool(this->children.getLast())); } };

struct TOpBitwiseOrI : TOperation
{ char* Exec() final { return res(get_int(this->children.getFirst()) |
                                         get_int(this->children.getLast())); } };

struct TOpLogicOr : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) ||
                                         get_bool(this->children.getLast())); } };

struct TOpLogicAnd : TOperation
{ char* Exec() final { return res(get_bool(this->children.getFirst()) &&
                                         get_bool(this->children.getLast())); } };

// ==================================================================================
// other

struct TOpTernary : TOperation
{
    TNode *last_node = nullptr;
    char* Exec() final
    {
        last_node = get_bool(this->children.getFirst())?
                      this->children.get(1):
                      this->children.getLast();
        return last_node->Exec();
    }
    void free_my_mem(void *mem) override
        {last_node->free_my_mem(mem);}
};

struct TOpAssign : TAssignationOperation
{
    char* Exec() final
    {
        char* left = this->children.getFirst()->Exec();
        char* right = this->children.getLast()->Exec();
        memcpy(left, right, operands_size);
        this->children.getLast()->free_my_mem(right);
        return left;
    }
};

struct TOpComma : TOperation
{
    char* Exec() final
    {
        this->children.getFirst()->ExecAndFree();
        return this->children.getLast()->Exec();
    }
    void free_my_mem(void *mem) override
        { this->children.getLast()->free_my_mem(mem);}
};

// ==================================================================================

TOperation* OperationManager::GetTOperation(InterpreterTNodeType type)
{
    switch (type)
    {
        case CastPointerToPointer:
            return new TOpCastPtoP;
        case CastPointerToBool:
            return new TOpCastPtoB;
        case CastCharToBool:
            return new TOpCastCtoB;
        case CastCharToInt:
            return new TOpCastCtoI;
        case CastCharToDouble:
            return new TOpCastCtoD;
        case CastIntToBool:
            return new TOpCastItoB;
        case CastIntToChar:
            return new TOpCastItoC;
        case CastIntToDouble:
            return new TOpCastItoD;
        case CastDoubleToBool:
            return new TOpCastDtoB;
        case CastDoubleToChar:
            return new TOpCastDtoC;
        case CastDoubleToInt:
            return new TOpCastDtoI;
            
        case PrefixIncPointer:
            return new TOpPreIncP;
        case PrefixIncInt:
            return new TOpPreIncI;
        case PrefixIncDouble:
            return new TOpPreIncD;
        case PostfixIncPointer:
            return new TOpPostIncP;
        case PostfixIncInt:
            return new TOpPostIncI;
        case PostfixIncDouble:
            return new TOpPostIncD;
        case PrefixDecPointer:
            return new TOpPreDecP;
        case PrefixDecInt:
            return new TOpPreDecI;
        case PrefixDecDouble:
            return new TOpPreDecD;
        case PostfixDecPointer:
            return new TOpPostDecP;
        case PostfixDecInt:
            return new TOpPostDecI;
        case PostfixDecDouble:
            return new TOpPostDecD;

        case UnaryPlusInt:
            return new TOpUnaryPlusI;
        case UnaryPlusDouble:
            return new TOpUnaryPlusD;
        case UnaryMinusInt:
            return new TOpUnaryMinusI;
        case UnaryMinusDouble:
            return new TOpUnaryMinusD;
        case NotBool:
            return new TOpNotBool;
        case BitwiseNotBool:
            return new TOpBitwiseNotB;
        case BitwiseNotInt:
            return new TOpBitwiseNotI;
        case Dereference:
            return new TOpDereference;
        case Reference:
            return new TOpReference;

        case MultiplyInts:
            return new TOpMulI;
        case MultiplyDoubles:
            return new TOpMulD;
        case DivideInts:
            return new TOpDivI;
        case DivideDoubles:
            return new TOpDivD;
        case ModInts:
            return new TOpModI;
        case BinaryPlusInts:
            return new TOpBinPlusI;
        case BinaryPlusDoubles:
            return new TOpBinPlusD;
        case BinaryPlusIntPointer:
            return new TOpBinPlusP;
        case BinaryMinusInts:
            return new TOpBinMinusI;
        case BinaryMinusDoubles:
            return new TOpBinMinusD;
        case BinaryMinusIntPointer:
            return new TOpBinMinusP;
        case ShiftRightInts:
            return new TOpShiftRight;
        case ShiftLeftInts:
            return new TOpShiftLeft;
        case CmpLessPointers:
            return new TOpCmpLessP;
        case CmpLessDoubles:
            return new TOpCmpLessD;
        case CmpMorePointers:
            return new TOpCmpMoreP;
        case CmpMoreDoubles:
            return new TOpCmpMoreD;
        case CmpLessEqualPointers:
            return new TOpCmpLessEqualP;
        case CmpLessEqualDoubles:
            return new TOpCmpLessEqualD;
        case CmpMoreEqualPointers:
            return new TOpCmpMoreEqualP;
        case CmpMoreEqualDoubles:
            return new TOpCmpMoreEqualD;
        case CmpEqualPointers:
            return new TOpCmpEqualP;
        case CmpEqualBools:
            return new TOpCmpEqualB;
        case CmpEqualDoubles:
            return new TOpCmpEqualD;
        case CmpNotEqualPointers:
            return new TOpCmpNotEqualP;
        case CmpNotEqualBools:
            return new TOpCmpNotEqualB;
        case CmpNotEqualDoubles:
            return new TOpCmpNotEqualD;
        case BitwiseAndBools:
            return new TOpBitwiseAndB;
        case BitwiseAndInts:
            return new TOpBitwiseAndI;
        case BitwiseXorBools:
            return new TOpBitwiseXorB;
        case BitwiseXorInts:
            return new TOpBitwiseXorI;
        case BitwiseOrBools:
            return new TOpBitwiseOrB;
        case BitwiseOrInts:
            return new TOpBitwiseOrI;
        case LogicOrBools:
            return new TOpLogicOr;
        case LogicAndBools:
            return new TOpLogicAnd;

        case Ternary:
            return new TOpTernary;
        case Assignment:
            return new TOpAssign;
        case Comma:
            return new TOpComma;
        
        default:
            return nullptr;
    }
    return nullptr;
}




