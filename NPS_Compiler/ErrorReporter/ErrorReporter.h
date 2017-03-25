//
// Created by Alexander on 25-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_ERRORREPORTER_H
#define NPS_C_TRANSLATOR_ERRORREPORTER_H

struct Error
{
    const char *message;
    const char *error_pos;
};

void ReportError(const char *error_pos, const char *message);
bool ErrorReported();
Error* GetError();


#endif //NPS_C_TRANSLATOR_ERRORREPORTER_H
