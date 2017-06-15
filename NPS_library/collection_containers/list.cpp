#include "list.h"
#include <cstring>
#include <algorithm>

BaseList::~BaseList()
{
    Segment *cur = first;
    while (cur)
    {
        Heap::free_mem(cur->data);
        Segment *next = cur->next;
        Heap::free_mem(cur);
        cur = next;
    }
}

void BaseList::add(const void *data)
{
    int offset = last_index++ % element_count;
    if (offset == 0)
        create_new_segment();
    memcpy(last->data + offset * element_size, data, element_size);
}

void* BaseList::get(int pos) const
{
    pos += first_index;
    if (pos >= last_index)
        return nullptr;
    int offset = pos % element_count;
    Segment *cur;
    // find segment from start or from end
    if (pos < last_index - pos)
    {
        cur = first;
        for (; pos >= element_count; pos -= element_count)
            cur = cur->next;
    }
    else
    {
        cur = last;
        int last_segment_no = (last_index - 1) / element_count;
        int search_segment_no = pos / element_count;
        for (; last_segment_no > search_segment_no; last_segment_no--)
            cur = cur->prev;
    }
    return cur->data + offset * element_size;
}

void* BaseList::get_first() const
{
    return first_index != last_index
           ? first->data + first_index * element_size
           : nullptr;
}

void* BaseList::get_last() const
{
    return first_index != last_index
           ? last->data + ((last_index - 1) % element_count) * element_size
           : nullptr;
}

void BaseList::take_first(void *store)
{
    if (first_index == last_index)
        return;
    if (store != nullptr)
        memcpy(store, first->data + first_index * element_size, element_size);
    increase_first_index();
}

void BaseList::take_last(void *store)
{
    if (first_index == -1 || first_index == last_index)
        return;
    if (store != nullptr)
        memcpy(store, last->data + ((last_index - 1) % element_count) * element_size, element_size);
    decrease_last_index();
}

void BaseList::take(int pos, void *store)
{
    pos += first_index;
    if (first_index == -1 || pos >= last_index)
        return;
    Segment *cur;
    char temp[element_size], boundary[element_size];
    
    // find element from start or from end
    if (pos < last_index - pos) // from start
    {
        // move forward all segments' data by 1 element until searching segment
        cur = first;
        for (int seg_last_index = element_count - 1;
             seg_last_index < pos; seg_last_index += element_count)
        {
            memcpy(temp, cur->data + (element_count - 1) * element_size, element_size);
            memmove(cur->data + element_size, cur->data, (element_count - 1) * element_size);
            memcpy(cur->data, boundary, element_size);
            std::swap(temp, boundary);
            cur = cur->next;
        }
        // move all data of searching segment until searching element
        int offset = pos % element_count;
        if (store != nullptr)
            memcpy(store, cur->data + offset * element_size, element_size);
        memmove(cur->data + element_size, cur->data, offset * element_size);
        memcpy(cur->data, boundary, element_size);
        
        increase_first_index();
    }
    else // find element from end
    {
        // move back all segments' data by 1 element until searching segment
        cur = last;
        for (int seg_first_index = element_count * ((last_index - 1) / element_count);
             seg_first_index > pos; seg_first_index -= element_count)
        {
            memcpy(temp, cur->data, element_size);
            memmove(cur->data, cur->data + element_size, (element_count - 1) * element_size);
            memcpy(cur->data + (element_count - 1) * element_size, boundary, element_size);
            std::swap(temp, boundary);
            cur = cur->prev;
        }
        
        // move all data of searching segment until searching element
        int offset = pos % element_count;
        char *element = cur->data + offset * element_size;
        if (store != nullptr)
            memcpy(store, element, element_size);
        memmove(element, element + element_size, (element_count - offset - 1) * element_size);
        memcpy(cur->data + (element_count - 1) * element_size, boundary, element_size);
        
        decrease_last_index();
    }
}

void BaseList::create_new_segment()
{
    Segment *segment = reinterpret_cast<Segment*>(Heap::get_mem(sizeof(Segment)));
    segment->data = Heap::get_mem(element_size * element_count);
    segment->next = nullptr;
    segment->prev = last;
    if (!first)
        first = segment;
    if (!last)
        last = segment;
    else
    {
        last->next = segment;
        segment->prev = last;
        last = segment;
    }
}

void BaseList::increase_first_index()
{
    if (++first_index == last_index)
    {
        first = last = nullptr;
        first_index = last_index = 0;
    }
    else if (first_index == element_count)
    {
        first_index = 0;
        last_index -= element_count;
        first = first->next;
        first->prev = nullptr;
    }
    else
        return;
    Heap::free_mem(first->data);
    Heap::free_mem(first);
}

void BaseList::decrease_last_index()
{
    if (--last_index == first_index)
    {
        first = last = nullptr;
        first_index = last_index = 0;
    }
    else if (last_index % element_count == 0)
    {
        last = last->prev;
        last->next = nullptr;
    }
    else
        return;
    Heap::free_mem(last->data);
    Heap::free_mem(last);
}

int BaseList::count() const
{ return last_index - first_index; }