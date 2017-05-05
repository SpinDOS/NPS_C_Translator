//
// Created by Alexander on 02-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_THASHTABLE_H
#define NPS_C_TRANSLATOR_THASHTABLE_H

#include "hash.h"
#include "cstring"

template <typename T> class THashTable : private Hash
{
    bool _error  = false;
    struct HashEntry
    {
        char *key;
        T *value;
    };

public:
    THashTable(int _n1, int _n2 = 0, int _n3 = 0)
            : Hash(_n1,_n2, _n3, 0, 0) {}
    
    T* get(const char *key);
    void put(const char *key, T *value);
    T* remove(const char *key);
    bool error() {return _error;}
    int key1(const char *key_word) final;
    int key2(const char *key_word) final;
    int key3(const char *key_word) final;
    
    ~THashTable();

protected:
    using Hash::element_count;
    using Hash::table;
};

#include "THashTable.tpp"

#endif //NPS_C_TRANSLATOR_THASHTABLE_H
