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
{ ReturnResult Exec() final { return res(get_int(this->children.getFirst()) != '\0'); } };

struct TOpCastDtoB : TOperation
{ ReturnResult Exec() final { return res(get_double(this->children.getFirst()) != '\0'); } };


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
        case InterpreterTNodeType::KeywordDoWhile:
        case InterpreterTNodeType::KeywordWhile:
        case InterpreterTNodeType::KeywordFor:
        case InterpreterTNodeType::KeywordIf:
        case InterpreterTNodeType::KeywordSwitch:
        case InterpreterTNodeType::KeywordBreak:
        case InterpreterTNodeType::KeywordContinue:
        case InterpreterTNodeType::KeywordReturn:
            return GetTKeyword(type);
    
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
        case PrefixIncInt:
        case PrefixIncDouble:
        case PostfixIncPointer:
        case PostfixIncInt:
        case PostfixIncDouble:
        case PrefixDecPointer:
        case PrefixDecInt:
        case PrefixDecDouble:
        case PostfixDecPointer:
        case PostfixDecInt:
        case PostfixDecDouble:
        case UnaryPlusInt:
        case UnaryPlusDouble:
        case UnaryMinusInt:
        case UnaryMinusDouble:
        case NotBool:
        case BitwiseNotBool:
        case BitwiseNotInt:
        case Dereference:
        case Reference:
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




