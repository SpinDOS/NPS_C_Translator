#ifndef __STRING_HELPERS_H_INCLUDED__
#define __STRING_HELPERS_H_INCLUDED__

char* copy_string(const char *s)
{
    if (!s)
        return nullptr;
    const char *temp = s;
    while (*temp++)
        ;
    return copy_string(s, temp - s - 1);
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

#endif  // __STRING_HELPERS_H_INCLUDED__
