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
    ReturnResult result = node->Exec();
    void *data = *reinterpret_cast<char**>(result.data);
    result.FreeIfNeed();
    return data;
}

char get_bool(TNode *node)
{
    ReturnResult result = node->Exec();
    bool data = *reinterpret_cast<bool*>(result.data);
    result.FreeIfNeed();
    return data;
}

char get_char(TNode *node)
{
    ReturnResult result = node->Exec();
    char data = *result.data;
    result.FreeIfNeed();
    return data;
}

int get_int(TNode *node)
{
    ReturnResult result = node->Exec();
    int data = *reinterpret_cast<int*>(result.data);
    result.FreeIfNeed();
    return data;
}

double get_double(TNode *node)
{
    ReturnResult result = node->Exec();
    double data = *reinterpret_cast<double*>(result.data);
    result.FreeIfNeed();
    return data;
}

ReturnResult res(void *p)
{
    ReturnResult result(Heap::get_mem(sizeof(void*)));
    *reinterpret_cast<void**>(result.data) = p;
    return result;
}

ReturnResult res(bool b)
{
    ReturnResult result(Heap::get_mem(sizeof(bool)));
    *reinterpret_cast<bool*>(result.data) = b;
    return result;
}

ReturnResult res(char ch)
{
    ReturnResult result(Heap::get_mem(sizeof(char)));
    *result.data = ch;
    return result;
}

ReturnResult res(int i)
{
    ReturnResult result(Heap::get_mem(sizeof(int)));
    *reinterpret_cast<int*>(result.data) = i;
    return result;
}

ReturnResult res(double d)
{
    ReturnResult result(Heap::get_mem(sizeof(double)));
    *reinterpret_cast<double*>(result.data) = d;
    return result;
}

// =======================================================================
// casts

struct TOpCastPtoP : TOperation
{ ReturnResult Exec() final { return this->children.getFirst()->Exec(); } };


struct TOpCastPtoB : TOperation
{ ReturnResult Exec() final { return res(get_pointer(this->children.getFirst()) != nullptr); } };

struct TOpCastCtoB : TOperation
{ ReturnResult Exec() final { return res(get_char(this->children.getFirst()) != '\0'); } };

struct TOpCastItoB : TOperation
{ ReturnResult Exec() final { return res(get_int(this->children.getFirst()) != 0); } };

struct TOpCastDtoB : TOperation
{ ReturnResult Exec() final { return res(get_double(this->children.getFirst()) != 0.0); } };


struct TOpCastItoC : TOperation
{ ReturnResult Exec() final { return res((char)get_int(this->children.getFirst()));} };

struct TOpCastDtoC : TOperation
{ ReturnResult Exec() final { return res((char)get_double(this->children.getFirst()));} };


struct TOpCastCtoI : TOperation
{ ReturnResult Exec() final { return res((int)get_char(this->children.getFirst()));} };

struct TOpCastDtoI : TOperation
{ ReturnResult Exec() final { return res((int)get_double(this->children.getFirst()));} };


struct TOpCastCtoD : TOperation
{ ReturnResult Exec() final { return res((double)get_char(this->children.getFirst()));} };

struct TOpCastItoD : TOperation
{ ReturnResult Exec() final { return res((double)get_int(this->children.getFirst()));} };

// ==================================================================================
// increment/decrement

ReturnResult prefix_int_inc_dec(TOperation *operation, int dif)
{
    ReturnResult param = operation->children.getFirst()->Exec();
    *reinterpret_cast<int *>(param.data) += dif;
    return param;
}

struct TOpPreIncI : TOperation
{ ReturnResult Exec() final { return prefix_int_inc_dec(this, 1); } };

struct TOpPreDecI : TOperation
{ ReturnResult Exec() final { return prefix_int_inc_dec(this, -1); } };

ReturnResult postfix_int_inc_dec(TOperation *operation, int dif)
{
    ReturnResult param = operation->children.getFirst()->Exec();
    int *param_value = reinterpret_cast<int*>(param.data);
    ReturnResult result(Heap::get_mem(sizeof(int)));
    *reinterpret_cast<int*>(result.data) = *param_value;
    *param_value += dif;
    return result;
}

struct TOpPostIncI : TOperation
{ ReturnResult Exec() final { return postfix_int_inc_dec(this, 1); } };

struct TOpPostDecI : TOperation
{ ReturnResult Exec() final { return postfix_int_inc_dec(this, -1); } };


ReturnResult prefix_double_inc_dec(TOperation *operation, double dif)
{
    ReturnResult param = operation->children.getFirst()->Exec();
    *reinterpret_cast<double *>(param.data) += dif;
    return param;
}

struct TOpPreIncD : TOperation
{ ReturnResult Exec() final { return prefix_double_inc_dec(this, 1.0); } };

struct TOpPreDecD : TOperation
{ ReturnResult Exec() final { return prefix_double_inc_dec(this, -1.0); } };

ReturnResult postfix_double_inc_dec(TOperation *operation, double dif)
{
    ReturnResult param = operation->children.getFirst()->Exec();
    double *param_value = reinterpret_cast<double*>(param.data);
    ReturnResult result(Heap::get_mem(sizeof(double)));
    *reinterpret_cast<double*>(result.data) = *param_value;
    *param_value += dif;
    return result;
}

struct TOpPostIncD : TOperation
{ ReturnResult Exec() final { return postfix_double_inc_dec(this, 1.0); } };

struct TOpPostDecD : TOperation
{ ReturnResult Exec() final { return postfix_double_inc_dec(this, -1.0); } };


ReturnResult prefix_pointer_inc_dec(TOperation *operation, int dif)
{
    ReturnResult param = operation->children.getFirst()->Exec();
    *reinterpret_cast<char**>(param.data) += dif;
    return param;
}

struct TOpPreIncP : TOperation
{ ReturnResult Exec() final { return prefix_pointer_inc_dec(this, size); } };

struct TOpPreDecP : TOperation
{ ReturnResult Exec() final { return prefix_pointer_inc_dec(this, -size); } };

ReturnResult postfix_pointer_inc_dec(TOperation *operation, int dif)
{
    ReturnResult param = operation->children.getFirst()->Exec();
    char **param_value = reinterpret_cast<char**>(param.data);
    ReturnResult result(Heap::get_mem(sizeof(void*)));
    *reinterpret_cast<char**>(result.data) = *param_value;
    *param_value += dif;
    return result;
}

struct TOpPostIncP : TOperation
{ ReturnResult Exec() final { return postfix_pointer_inc_dec(this, size); } };

struct TOpPostDecP : TOperation
{ ReturnResult Exec() final { return postfix_pointer_inc_dec(this, -size); } };

// ==================================================================================
// unary ops

struct TOpUnaryPlusI : TOperation
{ ReturnResult Exec() final { return res(get_int(this->children.getFirst())); } };

struct TOpUnaryPlusD : TOperation
{ ReturnResult Exec() final { return res(get_double(this->children.getFirst())); } };

struct TOpUnaryMinusI : TOperation
{ ReturnResult Exec() final { return res(-get_int(this->children.getFirst())); } };

struct TOpUnaryMinusD : TOperation
{ ReturnResult Exec() final { return res(-get_double(this->children.getFirst())); } };


struct TOpNotBool : TOperation
{ ReturnResult Exec() final { return res(!get_bool(this->children.getFirst())); } };

struct TOpBitwiseNotB : TOperation
{ ReturnResult Exec() final { return res(~get_bool(this->children.getFirst())); } };

struct TOpBitwiseNotI : TOperation
{ ReturnResult Exec() final { return res(~get_int(this->children.getFirst())); } };

struct TOpDereference : TOperation
{
    ReturnResult Exec() final
    {
        ReturnResult param = this->children.getFirst()->Exec();
        char *pointer = *reinterpret_cast<char**>(param.data);
        param.FreeIfNeed();
        void *underlying = Heap::get_mem(this->size);
        memcpy(underlying, pointer, this->size);
        return ReturnResult(underlying);
    }
};

struct TOpReference : TOperation
{
    ReturnResult Exec() final
    {
        ReturnResult param = this->children.getFirst()->Exec();
        void *pointer = Heap::get_mem(sizeof(void*));
        *reinterpret_cast<char**>(pointer) = param.data;
        return ReturnResult(pointer);
    }
};

// ==================================================================================
// other

struct TOpTernary : TOperation
{
    ReturnResult Exec() final
    {
        return get_bool(this->children.getFirst())?
               this->children.get(1)->Exec():
               this->children.getLast()->Exec();
    }
};

struct TOpAssign : TOperation
{
    ReturnResult Exec() final
    {
        ReturnResult left = this->children.getFirst()->Exec();
        ReturnResult right = this->children.getLast()->Exec();
        memcpy(left.data,right.data, size);
        right.FreeIfNeed();
        return left;
    }
};

struct TOpComma : TOperation
{
    ReturnResult Exec() final
    {
        this->children.getFirst()->Exec().FreeIfNeed();
        return this->children.getLast()->Exec();
    }
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
        case MultiplyDoubles:
        case DivideInts:
        case DivideDoubles:
        case ModInts:
        case BinaryPlusInts:
        case BinaryPlusDoubles:
        case BinaryPlusIntPointer:
        case BinaryMinusInts:
        case BinaryMinusDoubles:
        case BinaryMinusIntPointer:
        case ShiftRightInts:
        case ShiftLeftInts:
        case CmpLessPointers:
        case CmpLessDoubles:
        case CmpMorePointers:
        case CmpMoreDoubles:
        case CmpLessEqualPointers:
        case CmpLessEqualDoubles:
        case CmpMoreEqualPointers:
        case CmpMoreEqualDoubles:
        case CmpEqualPointers:
        case CmpEqualBools:
        case CmpEqualDoubles:
        case CmpNotEqualPointers:
        case CmpNotEqualBools:
        case CmpNotEqualDoubles:
        case BitwiseAndBools:
        case BitwiseAndInts:
        case BitwiseXorBools:
        case BitwiseXorInts:
        case BitwiseOrBools:
        case BitwiseOrInts:
        case LogicOrBools:
        case LogicAndBools:

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




