//
// Created by Alexander on 25-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_ERRORREPORTER_H
#define NPS_C_TRANSLATOR_ERRORREPORTER_H

struct Error
{
    const char *message;
    unsigned long error_pos;
};

void ReportError(unsigned long error_pos, const char *message);
bool ErrorReported();
Error* GetError();


#endif //NPS_C_TRANSLATOR_ERRORREPORTER_H
