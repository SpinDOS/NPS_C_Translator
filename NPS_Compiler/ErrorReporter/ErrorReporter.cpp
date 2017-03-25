//
// Created by Alexander on 25-Mar-17.
//

#include "ErrorReporter.h"
#include "../../NPS_library/heap/heap.h"

Error *error = nullptr;

void ReportError(const char *error_pos, const char *message)
{
    if (error != nullptr)
        return;

    error = (Error*)Heap::get_mem(sizeof(Error));
    error->error_pos = error_pos;
    error->message = message;
}

bool ErrorReported()
{ return error != nullptr; }

Error* GetError()
{return error;}