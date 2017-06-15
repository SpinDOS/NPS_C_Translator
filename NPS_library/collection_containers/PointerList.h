//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_SIMPLELINKEDLIST_H
#define NPS_C_TRANSLATOR_SIMPLELINKEDLIST_H

template <typename T> class PointerList
{
    struct Node
    {
        Node *prev, *next;
        T *data;
    };
    Node *first, *last;
    int _count;
    Node* find_node(int index) const;
public:
    PointerList(){ first = last = nullptr; _count = 0; }
    ~PointerList(){ clear(); }
    int count() const{ return _count;}
    void add (T *data);
    void insertBefore (T *data, int index);
    void insertAfter (T *data, int index);
    T* get(int index) const;
    T* getFirst() const;
    T* getLast() const;
    T* take(int index);
    T* takeFirst();
    T* takeLast();
    T** toArray(int &length);
    void clear();
};

#include "PointerList.tpp"

#endif //NPS_C_TRANSLATOR_SIMPLELINKEDLIST_H
