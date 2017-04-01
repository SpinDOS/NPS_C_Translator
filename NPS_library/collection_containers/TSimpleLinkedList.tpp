//
// Created by Alexander on 28-Mar-17.
//

#include "TSimpleLinkedList.h"
#include "../heap/heap.h"

template <typename T> void TSimpleLinkedList<T>::add(T data)
{
    _error = false;
    Node *node = static_cast<Node*>(Heap::get_mem(sizeof(Node)));
    node->data = data;
    node->next = nullptr;
    node->prev = last;
    if (last != nullptr)
        last->next = node;
    last = node;
    if (_count++ == 0)
        first = node;
}

template <typename T> void TSimpleLinkedList<T>::insertAfter(T data, int index)
{
    Node *node = find_node(index);
    if (node == nullptr)
        return;
    _count++;
    Node *newNode = static_cast<Node*>(Heap::get_mem(sizeof(Node)));
    newNode->data = data;
    newNode->next = node->next;
    newNode->prev = node;
    if (node == last)
        last = newNode;
    else
        node->next->prev = newNode;
    node->next = newNode;
}

template <typename T> void TSimpleLinkedList<T>::insertBefore(T data, int index)
{
    Node *node = find_node(index);
    if (node == nullptr)
        return;
    _count++;
    Node *newNode = static_cast<Node*>(Heap::get_mem(sizeof(Node)));
    newNode->data = data;
    newNode->prev = node->prev;
    newNode->next = node;
    if (node == first)
        first = newNode;
    else
        node->prev->next = newNode;
    node->prev = newNode;
}


template <typename T> T TSimpleLinkedList<T>::get(int index)
{
    Node *node = find_node(index);
    if (node == nullptr)
    {
        T t;
        return t;
    }
    return node->data;
}

template <typename T> T TSimpleLinkedList<T>::getFirst()
{
    _error = false;
    if (first != nullptr)
        return first->data;
    _error = true;
    T t;
    return t;
}

template <typename T> T TSimpleLinkedList<T>::getLast()
{
    _error = false;
    if (last != nullptr)
        return last->data;
    _error = true;
    T t;
    return t;
}

template <typename T> T TSimpleLinkedList<T>::take(int index)
{
    Node *node = find_node(index);
    if (node == nullptr)
    {
        T t;
        return t;
    }
    _count--;
    if (node == last)
        last = node->prev;
    else
        node->next->prev = node->prev;

    if (node == first)
        first = node->next;
    else
        node->prev->next = node->next;
    T result = node->data;
    Heap::free_mem(node);
    return result;
}

template <typename T> T TSimpleLinkedList<T>::takeFirst()
{
    if (_count == 0)
    {
        _error = true;
        T t;
        return t;
    }
    _error = false;
    T data = first->data;
    Node *temp = first;
    if (--_count == 0)
        first = last = nullptr;
    else
    {
        first = first->next;
        first->prev = nullptr;
    }
    Heap::free_mem(temp);
    return data;
}

template <typename T> T TSimpleLinkedList<T>::takeLast()
{
    if (_count == 0)
    {
        _error = true;
        T t;
        return t;
    }
    _error = false;
    T data = last->data;
    Node *temp = last;
    if (--_count == 0)
        first = last = nullptr;
    else
    {
        last = last->prev;
        last->next = nullptr;
    }
    Heap::free_mem(temp);
    return data;
}

template <typename T> T* TSimpleLinkedList<T>::toArray(int &length)
{
    _error = false;
    length = _count;
    if (length == 0)
        return nullptr;
    T *array = static_cast<T*>(Heap::get_mem(sizeof(T) * length));
    Node *cur = first;
    for (int i = 0; i < length; i++, cur = cur->next)
        array[i] = cur->data;
    return array;
}


template <typename T> void TSimpleLinkedList<T>::clear()
{
    while(first != nullptr)
    {
        Node *temp = first->next;
        Heap::free_mem(first);
        first = temp;
    }
    first = last = nullptr;
    _count = 0;
    _error = false;
}

template <typename T>
typename TSimpleLinkedList<T>::Node* TSimpleLinkedList<T>::find_node(int index)
{
    if (index < 0 || _count <= index)
    {
        _error = true;
        return nullptr;
    }
    _error = false;

    Node *cur;
    if (_count - index > index) // search from start
    {
        cur = first;
        while (index-- > 0)
            cur = cur->next;
    }
    else // search from end
    {
        index = _count - index - 1;
        cur = last;
        while (index-- > 0)
            cur = cur->prev;
    }
    return cur;
}
