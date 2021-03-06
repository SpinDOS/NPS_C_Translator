//
// Created by Alexander on 02-Apr-17.
//

#include "THashTable.h"
#include "../utils/string_utils.h"

template <typename T>
int THashTable<T>::key1(const char *key_word)
{
    unsigned int h = 0;
    unsigned char *p;
    
    for(p = (unsigned char *)key_word; *p != '\0'; p++)
        h = 31 * h + *p;
    return h;
}

template <typename T>
int THashTable<T>::key2(const char *key_word)
{ return strlen(key_word); }

template <typename T>
int THashTable<T>::key3(const char *key_word)
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

template <typename T>
T* THashTable<T>::get(const char *key)
{
    List* list = find_list(key);
    int c = combine_keys(key);
    if(list)
    {
        HashEntry *entry = nullptr;
        for (int i = 0; i < list->count(); i++)
        {
            entry = static_cast<HashEntry*>(list->get(i));
            if (strcmp(entry->key, key) == 0)
            {
                _error = false;
                return entry->value;
            }
        }
    }
    _error = true;
    return nullptr;
}

template <typename T> void THashTable<T>::put(const char *key, T *value)
{
    _error = false;
    List* list = find_list(key);
    HashEntry* entry;
    if(!list)
    {
        list = new List(sizeof(HashEntry));
        add_list(key, list);
    }
    for(int i = 0; i < list->count(); i++)
    {
        entry = static_cast<HashEntry*>(list->get(i));
        if(compare_strings(key, entry->key))
        {
            entry->value = value;
            return;
        }
    }
    HashEntry temp;
    temp.key = copy_string(key);
    temp.value = value;
    list->add(&temp);
}

template <typename T> T* THashTable<T>::remove(const char *key)
{
    List *list = find_list(key);
    if(list)
    {
        HashEntry *entry;
        for (int i = 0; i < list->count(); i++)
        {
            entry = static_cast<HashEntry *>(list->get(i));
            if (compare_strings(entry->key, key))
            {
                _error = false;
                HashEntry temp;
                list->take(i, &temp);
                T* value = temp.value;
                Heap::free_mem(temp.key);
                return value;
            }
        }
    }
    _error = true;
    return nullptr;
}

template <typename T> THashTable<T>::~THashTable()
{
    for (int i = 0; i < element_count; i++)
    {
        List *l = table[i];
        if (!l)
            continue;
        for (int j = 0; j < l->count(); j++)
            Heap::free_mem(static_cast<HashEntry*>(l->get(j))->key);
    }
}