//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H
#define NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H

#include <exception>
#include <string>

using namespace std;

struct LexemeParsingException : exception
{
    char *error_start;
    string message;
};
#endif //NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H
