#include "../heap/heap.h"
#include <cstring>
#include "string_utils.h"

char* copy_string(const char *s)
{
    if (!s)
        return nullptr;
    unsigned long length = 0;
    const char *temp = s;
    while(*temp++)
        length++;
    return copy_string(s, length);
}

char* copy_string(const char *s, unsigned long length)
{
    if (!s)
        return nullptr;
    char *result = (char*) Heap::get_mem(length + 1);
    memcpy(result, s, length);
    result[length] = '\0';
    return result;
}

bool compare_strings(const char *s1, const char *s2)
{
    if (!s1 || !s2)
        return !s1 && !s2;
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return *s1 == '\0' && *s2 == '\0';
}

