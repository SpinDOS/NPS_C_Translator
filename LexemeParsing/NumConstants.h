//
// Created by Alexander on 12-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_NUMCONSTANTS_H
#define NPS_C_TRANSLATOR_NUMCONSTANTS_H

enum NumType
{
    NumChar = 1,
    NumInt,
    NumDouble,
};
struct NumConstantDescription
{
    double num;
    NumType type;
};

#endif //NPS_C_TRANSLATOR_NUMCONSTANTS_H
