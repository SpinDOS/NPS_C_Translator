#include "list.h"
#include <cstring>
#include <algorithm>
Heap heap;

int List::count()
{
    if (first_index == -1)
    {
        _error = true;
        return -1;
    }
    return last_index - first_index;
}

void List::new_segment()
{
    Segment *segment = new Segment;
    segment->data = heap.get_mem(element_size * element_count);
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

void List::delete_segment(Segment* seg)
{
    if (seg == first && seg == last)
    {
        first = last = nullptr;
    }
    else
    {
        if (seg != last)
        {
            seg->next->prev = seg->prev;
        }
        else
        {
            last = seg->prev;
            last->next = nullptr;
        }

        if (seg != first)
        {
            seg->prev->next = seg->next;
        }
        else
        {
            first = seg->next;
            first->prev = nullptr;
        }
    }
    heap.free_mem(seg->data);
    delete seg;
}

List::~List()
{
    first_index = last_index = -1;
    Segment *cur = first;
    while (cur)
    {
        heap.free_mem(cur->data);
        Segment *next = cur->next;
        delete cur;
        cur = next;
    }
}

List::List(int _element_size, int _element_count)
{
    element_size = _element_size;
	element_count = _element_count;
	first_index = last_index = 0;
	first = last = nullptr;
}

void List::add(void *data)
{
    if (first_index == -1)
    {
        _error = true;
        return;
    }
    _error = false;
    int offset = last_index++ % element_count;
    if (!offset)
        new_segment();
    memcpy((char*)last->data + offset * element_size, data, element_size);
}

void* List::get(int pos)
{
    pos += first_index;
    if (first_index == -1 || pos >= last_index)
    {
        _error = true;
        return nullptr;
    }
    _error = false;
    int offset = pos % element_count;
    Segment *cur;
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
    void *temp = heap.get_mem(element_size);
    memcpy(temp, (char*)cur->data + offset * element_size, element_size);
    return temp;
}


void List::take_first(void *store)
{
    if (first_index == -1 || first_index == last_index)
    {
        _error = true;
        return;
    }
    _error = false;
    memcpy(store, (char*)first->data + first_index * element_size, element_size);
    if (++first_index == element_count)
    {
        first_index = 0;
        last_index -= element_count;
        delete_segment(first);
    }
}

void List::take_last(void *store)
{
    if (first_index == -1 || first_index == last_index)
    {
        _error = true;
        return;
    }
    _error = false;
    memcpy(store, (char*)last->data + (last_index - 1)* element_size, element_size);
    if (--last_index == first_index)
    {
        delete_segment(first);
        last_index = first_index = 0;
    }
    else if (last_index % element_count == 0)
        delete_segment(last);
}

void List::take(int pos, void* store)
{
    pos += first_index;
    if (first_index == -1 || pos >= last_index)
    {
        _error = true;
        return;
    }
    _error = false;
    Segment *cur;
    char *temp = (char*) heap.get_mem(element_size);
    char *boundary = (char*) heap.get_mem(element_size);

    if (pos < last_index - pos)
    {
        cur = first;
        for (int seg_last_index = element_count - 1;
            seg_last_index < pos; seg_last_index += element_count)
        {
            memcpy(temp, (char*) cur->data + (element_count - 1) * element_size, element_size);
            memmove((char*)cur->data + element_size, cur->data, (element_count - 1) * element_size);
            memcpy(cur->data, boundary, element_size);
            std::swap(temp, boundary);
            cur = cur->next;
        }
        int offset = pos % element_count;
        memcpy(store, (char*) cur->data + offset * element_size, element_size);
        memmove((char*)cur->data + element_size, cur->data, offset * element_size);
        memcpy(cur->data, boundary, element_size);

        if (++first_index == element_count)
        {
            first_index = 0;
            last_index -= element_count;
            delete_segment(first);
        }
    }
    else
    {
        cur = last;
        for (int seg_first_index = element_count * ((last_index - 1) / element_count);
            seg_first_index > pos; seg_first_index -= element_count)
        {
            memcpy(temp, cur->data, element_size);
            memmove(cur->data, (char*)cur->data + element_size, (element_count - 1) * element_size);
            memcpy((char*) cur->data + (element_count - 1) * element_size, boundary, element_size);
            std::swap(temp, boundary);
            cur = cur->prev;
        }
        int offset = pos % element_count;
        char *element = (char*) cur->data + offset * element_size;
        memcpy(store, element, element_size);
        memmove(element, element + element_size, (element_count - offset - 1) * element_size);
        memcpy((char*) cur->data + (element_count - 1) * element_size, boundary, element_size);

        if (--last_index == first_index)
        {
            delete_segment(first);
            last_index = first_index = 0;
        }
        else if (last_index % element_count == 0)
            delete_segment(last);
    }
    heap.free_mem(temp);
    heap.free_mem(boundary);
}

void List::sort(bool dir)
{

}

