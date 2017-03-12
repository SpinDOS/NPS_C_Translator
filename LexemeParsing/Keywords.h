//
// Created by murchan on 3/12/17.
//

#ifndef NPS_C_TRANSLATOR_KEYWORDS_H
#define NPS_C_TRANSLATOR_KEYWORDS_H


class Keywords {
    const char *keywordsNoParentheses[67] = {
            "auto", "const", "double", "float", "int", "short",
            "struct", "unsigned", "break", "continue", "else",
            "long", "signed", "void", "case", "default", "enum",
            "goto", "register", "sizeof",
            "typedef", "volatile", "char", "do", "extern",
            "return", "static", "union", "asm",
            "dynamic_cast", "namespace", "reinterpret_cast", "try",
            "bool", "explicit", "new", "static_cast", "typeid",
            "false", "operator", "template", "typename",
            "class", "friend", "private", "this", "using",
            "const_cast", "inline", "public", "throw", "virtual",
            "mutable", "protected", "true", "wchar_t",
            "and", "bitand", "compl", "not_eq", "or_eq", "xor_eq",
            "and_eq", "bitor", "not", "or", "xor"
    };
    const char *keywordsWithParentheses[5] = {
            "for", "switch", "if", "while", "catch",
    };
    const char *keywordsSpecial[1] = {
            "delete"
    };
};


#endif //NPS_C_TRANSLATOR_KEYWORDS_H
