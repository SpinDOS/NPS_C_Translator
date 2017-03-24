//
// Created by user on 23.03.2017.
//

#ifndef NPS_C_TRANSLATOR_HASHTABLE_H
#define NPS_C_TRANSLATOR_HASHTABLE_H


#include "../collection_containers/hash.h"
#include "HashEntry.h"
#include <string.h>

struct Variable
{
    unsigned long offset;
    char* name;
    char* type;
};

class HashTable : public Hash{
public:
    HashTable() : Hash(103,5,0,0,0){}

    Variable* get(char *key);

    void put(char *key, Variable* variable);

    bool remove(char *key);

    int key1(const char *key_word)
    {
        unsigned int h = 0;
        unsigned char *p;

        for(p = (unsigned char *) key_word; *p != '\0'; p++)
            h = 31 * h + *p;
        return h % element_count;
    }
    int key2(const char *key_word) { return strlen(key_word); }
};


#endif //NPS_C_TRANSLATOR_HASHTABLE_H
