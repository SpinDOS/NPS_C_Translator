//
// Created by user on 23.03.2017.
//

#include <cstring>
#include "HashTable.h"

Variable* HashTable::get(char *key)
{
    List* list = find_list(key);
    if(!list)
        return nullptr;
    HashEntry<char, Variable>* entry = nullptr;
    for(int i = 0; i < list->count(); i++)
    {
        entry = (HashEntry<char, Variable>*)list->get(i);
        if(strcmp(entry->getKey(), key) == 0)
            return entry->getValue();
    }
    return nullptr;
}

void HashTable::put(char *key, Variable *variable)
{
    List* list = find_list(key);
    HashEntry<char, Variable>* entry;
    if(!list)
        list = (List*)Heap::get_mem(sizeof(List));
    for(int i = 0; i < list->count(); i++)
    {
        entry = (HashEntry<char, Variable>*)list->get(i);
        if(strcmp(key, entry->getKey()) == 0)
        {
            entry->setValue(variable);
            return;
        }
    }
    entry = (HashEntry<char, Variable>*)Heap::get_mem(sizeof(HashEntry<char, Variable>));
    entry->setKey(key);
    entry->setValue(variable);
    list->add(entry);
}
