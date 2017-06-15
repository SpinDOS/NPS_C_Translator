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

int Key1(const char *key_word)
{
    unsigned int h = 0;
    unsigned char *p;
    
    for(p = (unsigned char *)key_word; *p != '\0'; p++)
        h = 31 * h + *p;
    return h;
}

int Key2(const char *key_word)
{ return strlen(key_word); }

int Key3(const char *key_word)
{
    int result = 0;
    for (; *key_word; key_word++)
    {
        int temp = 0;
        for (int j = 0; j < 4 && *key_word; j++, key_word++)
            temp = temp << 1 + *key_word;
        result ^= temp;
    }
    return result;
}

#endif  // __STRING_HELPERS_H_INCLUDED__
