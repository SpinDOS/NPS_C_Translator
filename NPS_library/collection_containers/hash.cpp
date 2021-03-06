#include "hash.h"
#include "../utils/string_utils.h"
#include <algorithm>
#include <cstring>

#define MAXRANGE 20

Hash::Hash(int _n1, int _n2, int _n3, int _n4, int _n5)
{
    n1 = abs(_n1) % MAXRANGE + 1;
    n2 = abs(_n2) % MAXRANGE + 1;
    n3 = abs(_n3) % MAXRANGE + 1;
    n4 = abs(_n4) % MAXRANGE + 1;
    n5 = abs(_n5) % MAXRANGE + 1;
    element_count = n1*n2*n3*n4*n5;
    table=(List**)Heap::get_mem(element_count*sizeof(List*));
    memset(table, 0, element_count * sizeof(List*));
};

Hash::~Hash()
{
    for (int i = 0; i < element_count; i++)
    {
        List *list = table[i];
        if (list)
            delete list;
    }
    Heap::free_mem(table);
    table = nullptr;
}

int Hash::combine_keys(const char* key_word)
{
    return
            abs(key5(key_word))%n5 +
            abs(key4(key_word))%n4 *n5 +
            abs(key3(key_word))%n3 *n5*n4 +
            abs(key2(key_word))%n2 *n5*n4*n3 +
            abs(key1(key_word))%n1 *n5*n4*n3*n2;
};



Diction_list::~Diction_list()
{
    Article temp;
    for(int i = 0; i < count(); i++)
    {
        take_first(&temp);
        Heap::free_mem(temp.word);
        Heap::free_mem(temp.description);
    }
}

void Diction_list::put(Article *article)
{
    Article new_article;
    new_article.word = copy_string(article->word);
    new_article.description = copy_string(article->description);
    add(&new_article);
}

Article* Diction_list::find(const char *word)
{
    int c = count();
    for (int i = 0; i < c; i++)
    {
        Article *article = (Article*) get(i);
        if (compare_strings(article->word, word))
            return article;
    }
    return nullptr;
}

void Diction_list::del(const char *word)
{
    for (int i = 0; i < count(); i++)
    {
        Article *article = (Article*) get(i);
        if (compare_strings(article->word, word))
        {
            Article temp;
            take(i, &temp);
            Heap::free_mem(temp.word);
            Heap::free_mem(temp.description);
            return;
        }
    }
}



Article* Diction::auto_create(const char *word)
{
    Diction_list *list = (Diction_list *) find_list(word);
    if (!list)
    {
        list = new Diction_list;
        add_list(word, (List*) list);
    }
    Article *article = list->find(word);
    if (!article)
    {
        Article temp;
        temp.word = copy_string(word);
        temp.description = nullptr;
        list->put(&temp);
        article = list->find(word);
    }
    return article;
}
