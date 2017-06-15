#include "hash.h"
#include "../utils/string_utils.h"
#include <algorithm>
#include <cstring>

#define MAXRANGE 20

BaseHash::BaseHash(int _size_of_key, int _size_of_value, int _list_count,
                   int _n1, int _n2 = 0, int _n3 = 0, int _n4 = 0, int _n5 = 0)
{
    n1 = abs(_n1) % MAXRANGE + 1;
    n2 = abs(_n2) % MAXRANGE + 1;
    n3 = abs(_n3) % MAXRANGE + 1;
    n4 = abs(_n4) % MAXRANGE + 1;
    n5 = abs(_n5) % MAXRANGE + 1;
    element_count = n1*n2*n3*n4*n5;
    key_size = _size_of_key;
    value_size = _size_of_value;
    list_count = _list_count;
    table = (BaseList**)Heap::get_mem(element_count*sizeof(BaseList*));
    memset(table, 0, element_count * sizeof(BaseList*));
};

BaseHash::~BaseHash()
{
    for (int i = 0; i < element_count; i++)
    {
        BaseList *list = table[i];
        if (list)
            delete list;
    }
    Heap::free_mem(table);
}

int BaseHash::combine_keys(const char* key_word) const
{
    return
            abs(key5(key_word))%n5 +
            abs(key4(key_word))%n4 *n5 +
            abs(key3(key_word))%n3 *n5*n4 +
            abs(key2(key_word))%n2 *n5*n4*n3 +
            abs(key1(key_word))%n1 *n5*n4*n3*n2;
};

char* BaseHash::find_node_in_list(BaseList *list, const char *key) const
{
    for (int i = 0; i < list->count(); i++)
    {
        char *list_node = static_cast<char*>(list->get(i));
        if (key_compare(key, list_node))
            return list_node + key_size;
    }
    return nullptr;
}

void BaseHash::add(const char *key, const char *value)
{
    BaseList *list = find_list(key);
    if (list == nullptr)
    {
        list = new BaseList(key_size + value_size, list_count);
        table[combine_keys(key)] = list;
    }
    char *list_node = find_node_in_list(list, key);
    if (list_node)
        memcpy(list_node, value, value_size);
    else
    {
        char temp[key_size + value_size];
        memcpy(temp, key, key_size);
        memcpy(temp + key_size, value, value_size);
        list->add(temp);
    }
}

char* BaseHash::get(const char *key) const
{
    BaseList *list = find_list(key);
    if (!list)
        return nullptr;
    else
        return find_node_in_list(list, key);
}

void BaseHash::take(const char *key, char *store)
{
    BaseList *list = find_list(key);
    if (!list)
        return;
    for (int i = 0; i < list->count(); i++)
    {
        char *list_node = static_cast<char*>(list->get(i));
        if (!key_compare(key, list_node))
            continue;
        char temp[key_size + value_size];
        list->take(i, temp);
        memcpy(store, temp + key_size, value_size);
        return;
    }
}

