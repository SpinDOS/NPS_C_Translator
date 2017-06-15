//
// Created by Alexander on 28-Mar-17.
//

#include "PointerList.h"
#include "../heap/heap.h"

template <typename T> void PointerList<T>::add(T *data)
{
    Node *node = reinterpret_cast<Node*>(Heap::get_mem(sizeof(Node)));
    node->data = data;
    node->next = nullptr;
    node->prev = last;
    if (last != nullptr)
        last->next = node;
    last = node;
    if (_count++ == 0)
        first = node;
}

template <typename T> void PointerList<T>::insertBefore(T *data, int index)
{
    if (index < 0)
        index = 0;
    if (index >= _count)
    {
        add(data);
        return;
    }
    Node *node = find_node(index);
    _count++;
    Node *newNode = reinterpret_cast<Node*>(Heap::get_mem(sizeof(Node)));
    newNode->data = data;
    newNode->prev = node->prev;
    newNode->next = node;
    if (node == first)
        first = newNode;
    else
        node->prev->next = newNode;
    node->prev = newNode;
}

template <typename T> void PointerList<T>::insertAfter(T *data, int index)
{
    if (index < 0)
        index = 0;
    if (index >= _count)
    {
        add(data);
        return;
    }
    Node *node = find_node(index);
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

template <typename T> T* PointerList<T>::get(int index) const
{
    Node *node = find_node(index);
    if (node == nullptr)
        return nullptr;
    return node->data;
}

template <typename T> T* PointerList<T>::getFirst() const
{ return first? first->data : nullptr; }

template <typename T> T* PointerList<T>::getLast() const
{ return last? last->data : nullptr; }


template <typename T> T* PointerList<T>::take(int index)
{
    Node *node = find_node(index);
    if (node == nullptr)
        return nullptr;
    _count--;
    if (node == last)
        last = node->prev;
    else
        node->next->prev = node->prev;

    if (node == first)
        first = node->next;
    else
        node->prev->next = node->next;
    T *result = node->data;
    Heap::free_mem(node);
    return result;
}

template <typename T> T* PointerList<T>::takeFirst()
{
    if (_count == 0)
        return nullptr;
    T *data = first->data;
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

template <typename T> T* PointerList<T>::takeLast()
{
    if (_count == 0)
        return nullptr;
    T *data = last->data;
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

template <typename T> T** PointerList<T>::toArray(int &length)
{
    length = _count;
    if (length == 0)
        return nullptr;
    T **array = static_cast<T*>(Heap::get_mem(sizeof(T*) * length));
    Node *cur = first;
    for (int i = 0; i < length; i++, cur = cur->next)
        array[i] = cur->data;
    return array;
}


template <typename T> void PointerList<T>::clear()
{
    while(first != nullptr)
    {
        Node *temp = first->next;
        Heap::free_mem(first);
        first = temp;
    }
    first = last = nullptr;
    _count = 0;
}

template <typename T>
typename PointerList<T>::Node* PointerList<T>::find_node(int index) const
{
    if (index < 0 || _count <= index)
        return nullptr;

    Node *cur;
    if (index < _count - index) // search from start
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
