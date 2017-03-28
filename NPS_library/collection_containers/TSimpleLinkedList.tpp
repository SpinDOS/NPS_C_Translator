//
// Created by Alexander on 28-Mar-17.
//

#include "TSimpleLinkedList.h"
#include "../heap/heap.h"

template <typename T> void TSimpleLinkedList<T>::add(T data)
{
    Node *node = static_cast<Node*>(Heap::get_mem(sizeof(Node)));
    node->data = data;
    node->next = nullptr;
    node->prev = last;
    last = node;
    if (_count++ == 0)
        first = node;
}

template <typename T> T TSimpleLinkedList<T>::getFirst()
{
    if (first != 0)
        return first->data;
    T t;
    return t;
}
