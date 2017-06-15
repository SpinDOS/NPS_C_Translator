#ifndef __HASH_H_INCLUDED__
#define __HASH_H_INCLUDED__

#include <cstring>
#include "list.h"
#include "../utils/string_utils.h"

class BaseHash
{
public:
    void add(const char *key, const char *value);
    char* get(const char *key) const;
    void take(const char *key, char *store);
    virtual ~BaseHash();
protected:
	BaseHash(int _size_of_key, int _size_of_value, int _list_count = 32,
             int _n1, int _n2 = 0, int _n3 = 0, int _n4 = 0, int _n5 = 0);
	virtual int key1(const char *key_word) const { return 0; };
	virtual int key2(const char *key_word) const { return 0; };
	virtual int key3(const char *key_word) const { return 0; };
	virtual int key4(const char *key_word) const { return 0; };
	virtual int key5(const char *key_word) const { return 0; };
    virtual bool key_compare(const char *key1, const char *key2) const = 0 ;
    
    int element_count;
	BaseList **table;
private:
    int combine_keys(const char* key_word) const;
    BaseList* find_list(const char *key_word) const
        {return table[combine_keys(key_word)];}
    char *find_node_in_list(BaseList *list, const char *key) const;
    int    n1,n2,n3,n4,n5;
    int key_size, value_size;
    int list_count;
};

template <typename T> class Hash : public BaseHash
{
public:
    Hash(int _n1, int _n2 = 0, int _n3 = 0, int _list_count = 32) :
            BaseHash(sizeof(char*), sizeof(T), _list_count, _n1, _n2, _n3, 0, 0){}
    T* Get(const char *key) const
        { return reinterpret_cast<T*>(get(reinterpret_cast<char*>(&key))); }
    void Add(const char *key, const T &value)
        { add(reinterpret_cast<char*>(&key), &value); }
    T Take(const char *key)
        { T t; take(reinterpret_cast<char*>(&key), &t); return t; }
private:
    bool key_compare(const char *key1, const char *key2) const final
    { return strcmp(reinterpret_cast<char*>(*key1), reinterpret_cast<char*>(*key2)) == 0; }
    int key1(const char *key_word) const final { return Key1(reinterpret_cast<char*>(*key_word));}
    int key2(const char *key_word) const final { return Key2(reinterpret_cast<char*>(*key_word));}
    int key3(const char *key_word) const final { return Key3(reinterpret_cast<char*>(*key_word));}
};


#endif // __HASH_H_INCLUDED__