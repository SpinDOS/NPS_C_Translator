#include "heap.h"
#include "heap.h"
#include <cstring>

char* copy_string(const char *s)
{
    if (!s)
        return nullptr;
    int length = 1;
    const char *temp = s;
    while(*temp++)
        length++;
    char *result = (char*) Heap::get_mem(length);
    memcpy(result, s, length);
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
