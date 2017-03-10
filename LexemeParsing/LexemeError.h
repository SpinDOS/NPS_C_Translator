//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H
#define NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H

#include <string>

using namespace std;

struct LexemeError
{
    const char *error_start;
    string invalid_lexeme;
    string message;
};
#endif //NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H
