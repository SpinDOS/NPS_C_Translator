//
// Created by Alexander on 25-Mar-17.
//

#include "ErrorReporter.h"
#include "../../NPS_library/heap/heap.h"

Error *error = nullptr;

void ReportError(long error_pos, const char *message)
{
    if (error == nullptr)
        error = (Error*)Heap::get_mem(sizeof(Error));
    error->error_pos = error_pos;
    if (error->message != nullptr)
        Heap::free_mem(error->message);
    error->message = copy_string(message);
}

void ReportError(LexemeWord *lexeme, const char *message)
{ ReportError(lexeme->positionInTheText, message); }

bool ErrorReported()
{ return error != nullptr; }

Error* GetError()
{return error;}