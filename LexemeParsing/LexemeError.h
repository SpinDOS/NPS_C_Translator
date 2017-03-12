//
// Created by Alexander on 07-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H
#define NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H

struct LexemeError
{
    const char *error_start;
    const char *invalid_lexeme;
    const char *message;
};
#endif //NPS_C_TRANSLATOR_LEXEMEPARSINGERROR_H
