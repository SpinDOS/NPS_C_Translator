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
    char *result = Heap::get_mem(length + 1);
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

char* str_cat(const char *s1, const char *s2)
{
    unsigned int length1 = 0, length2 = 0;
    const char *temp = s1;
    while(*temp++)
        length1++;
    temp = s2;
    while(*temp++)
        length2++;
    char *result = Heap::get_mem(length1 + length2 + 1);
    memcpy(result, s1, length1);
    memcpy(result + length1, s2, length2);
    result[length1 + length2] = '\0';
    return result;
}

