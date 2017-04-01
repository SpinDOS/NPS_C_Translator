//
// Created by Alexander on 28-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_SIMPLELINKEDLIST_H
#define NPS_C_TRANSLATOR_SIMPLELINKEDLIST_H

template <typename T> class TSimpleLinkedList
{
    struct Node
    {
        Node *prev, *next;
        T data;
    };
    Node *first, *last;
    int _count;
    bool _error;
    Node* find_node(int index);
public:
    TSimpleLinkedList(){ first = last = nullptr; _count = 0; _error = false; }
    ~TSimpleLinkedList(){clear();}
    int count(){return _count;}
    bool error(){ return _error;}
    void add (T data);
    void insertBefore (T data, int index);
    void insertAfter (T data, int index);
    T get(int index);
    T getFirst();
    T getLast();
    T take(int index);
    T takeFirst();
    T takeLast();
    T* toArray(int &length);
    void clear();
};

#include "TSimpleLinkedList.tpp"

#endif //NPS_C_TRANSLATOR_SIMPLELINKEDLIST_H
