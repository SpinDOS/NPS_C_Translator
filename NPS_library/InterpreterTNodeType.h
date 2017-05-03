//
// Created by Alexander on 21-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_PRIMITIVEOPERATIONS_H
#define NPS_C_TRANSLATOR_PRIMITIVEOPERATIONS_H

namespace NPS_Interpreter
{
    enum InterpreterTNodeType : unsigned int
    {
        NotDefined,

        ListOfSentences,
        ParamsGetter, // (name, size)
        Declaration, // (name, size)

        Variable,
        Constant,

        KeywordIf,
        KeywordWhile,
        KeywordDoWhile,
        KeywordFor,
        KeywordSwitch,
        KeywordSwitchCase,
        KeywordBreak,
        KeywordContinue,
        KeywordReturn,

        FunctionCall,

        CastPointerToPointer, // may be redundant
        CastPointerToBool,
        CastCharToBool,
        CastCharToInt,
        CastCharToDouble,
        CastIntToBool,
        CastIntToChar,
        CastIntToDouble,
        CastDoubleToBool,
        CastDoubleToChar,
        CastDoubleToInt,

        Member, // (offset) // .

        PrefixIncPointer, // (size)
        PrefixIncInt,
        PrefixIncDouble,
        PostfixIncPointer, // (size)
        PostfixIncInt,
        PostfixIncDouble,

        PrefixDecPointer, // (size)
        PrefixDecInt,
        PrefixDecDouble,
        PostfixDecPointer, // (size)
        PostfixDecInt,
        PostfixDecDouble,

        UnaryPlusInt,
        UnaryPlusDouble,

        UnaryMinusInt,
        UnaryMinusDouble,

        NotBool,
        BitwiseNotBool,
        BitwiseNotInt,

        Dereference,
        Reference,

        MultiplyInts,
        MultiplyDoubles,

        DivideInts,
        DivideDoubles,

        ModInts,

        BinaryPlusInts,
        BinaryPlusDoubles,
        BinaryPlusIntPointer, // (size)

        BinaryMinusInts,
        BinaryMinusDoubles,
        BinaryMinusIntPointer, // (size)

        ShiftRightInts,
        ShiftLeftInts,

        CmpLessPointers,
        CmpLessDoubles,
        CmpMorePointers,
        CmpMoreDoubles,
        CmpLessEqualPointers,
        CmpLessEqualDoubles,
        CmpMoreEqualPointers,
        CmpMoreEqualDoubles,

        CmpEqualPointers,
        CmpEqualBools,
        CmpEqualDoubles,
        CmpNotEqualPointers,
        CmpNotEqualBools,
        CmpNotEqualDoubles,

        BitwiseAndBools,
        BitwiseAndInts,
        BitwiseXorBools,
        BitwiseXorInts,
        BitwiseOrBools,
        BitwiseOrInts,

        LogicOrBools,
        LogicAndBools,

        Ternary,
        Assignment, // (size)
        Comma,
    };
    
    enum TConstantType
    {
        TypePointer,
        TypeBool,
        TypeChar,
        TypeString,
        TypeInt,
        TypeDouble,
    };
}

#endif //NPS_C_TRANSLATOR_PRIMITIVEOPERATIONS_H
