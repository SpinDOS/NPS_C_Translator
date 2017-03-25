#include "../utils/string_utils.h"
#include "HashTable.h"

Variable* HashTable::get(char *key)
{
    List* list = find_list(key);
    if(!list)
        return nullptr;
    HashEntry<char, Variable>* entry = nullptr;
    for(int i = 0; i < list->count(); i++)
    {
        entry = static_cast<HashEntry<char, Variable>*>(list->get(i));
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
    {
        list = new List(sizeof(HashEntry<char, Variable>));
        add_list(key, list);
    }
    for(int i = 0; i < list->count(); i++)
    {
        entry = static_cast<HashEntry<char, Variable>*>(list->get(i));
        if(compare_strings(key, entry->getKey()) == 0)
        {
            entry->setValue(variable);
            return;
        }
    }
    entry = static_cast<HashEntry<char, Variable>*>(Heap::get_mem(sizeof(HashEntry<char, Variable>)));
    entry->setKey(key);
    entry->setValue(variable);
    list->add(entry);
}

bool HashTable::remove(char *key)
{
    List *list = find_list(key);
    if(!list)
        return false;
    HashEntry<char,Variable> *entry;
    for(int i = 0; i < list->count(); i++)
    {
        entry = static_cast<HashEntry<char, Variable>*>(list->get(i));
        if(compare_strings(entry->getKey(), key) == 0)
        {
            list->take(i, entry);
            return true;
        }
    }
    return false;
}