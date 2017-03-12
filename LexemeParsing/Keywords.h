//
// Created by murchan on 3/12/17.
//

#ifndef NPS_C_TRANSLATOR_KEYWORDS_H
#define NPS_C_TRANSLATOR_KEYWORDS_H


const char *keywords[11][7] = {
        //Loop operators
        {"for", "while", "do", "break", "continue"},
        //RETURN
        {"return"},
        //IF-ELSE
        {"if", "else"},
        //SWITCH-CASE-DEFAULT
        {"switch", "case", "default"},
        //True-false
        {"true", "false"},
        //Access modifiers
        {"private", "public", "protected"},
        //Structure types
        {"class", "struct", "enum", "namespace"},
        //Variable types
        {"void", "char", "bool", "double", "int"},
        //CONST-STATIC
        {"const", "static"},
        //VIRTUAL
        {"virtual"},
        //Other functions
        {"input", "output", "sin", "cos", "max", "min", "new"}
};


#endif //NPS_C_TRANSLATOR_KEYWORDS_H
