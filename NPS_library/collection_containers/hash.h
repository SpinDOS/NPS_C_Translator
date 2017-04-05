#ifndef __HASH_H_INCLUDED__
#define __HASH_H_INCLUDED__

#include "list.h"

class Hash
{
public:
	Hash(int _n1, int _n2 = 0, int _n3 = 0, int _n4 = 0, int _n5 = 0);
	virtual ~Hash();

	List* find_list(const char *key_word)
          { return table[combine_keys(key_word)]; };

	virtual int key1(const char *key_word) { return 0; };
	virtual int key2(const char *key_word) { return 0; };
	virtual int key3(const char *key_word) { return 0; };
	virtual int key4(const char *key_word) { return 0; };
	virtual int key5(const char *key_word) { return 0; };

protected:
    int element_count;
    int combine_keys(const char* key_word);
    List* add_list(const char *key_word, List *list){
        table[combine_keys(key_word)] = list;
        return list;
    }
	List **table;
	int    n1,n2,n3,n4,n5;
};

struct Article
{
	char *word;
	char *description;
};

class Diction_list : protected List
{
public:
	Diction_list() : List(sizeof(Article)){}
    virtual ~Diction_list(); // purge all Article data

	void     put(Article *article); // allocate memory for word & description
	Article* find(const char *word);
	void     del(const char *word); // free memory from word & description
	void     del(const Article *article) { del(article->word); };
    
    using List::error;
};

class Diction : protected Hash
{
public: 
	Diction() :Hash(33,33,0,0,0){ }
	
	int key1(const char *key_word) { int f = key_word[0]-'A';
				   if (f<33 && f>0) return f;
                   else             return 0; }
	int key2(const char *key_word) { return key_word[1] % 33; }

	Article* find (const char *word)
    { Diction_list *list =(Diction_list*)find_list(word);
      return list ? list->find(word) : nullptr;
    };
	Article* auto_create(const char* word); // create Article (and List) if not exist
	void del(const char *word)
    { Diction_list *list =(Diction_list*)find_list(word);
        if (list)
            list->del(word);
    };
};

#endif // __HASH_H_INCLUDED__