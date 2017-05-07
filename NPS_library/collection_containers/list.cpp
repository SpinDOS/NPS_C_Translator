#include "list.h"
#include <cstring>
#include <algorithm>

List::List(int _element_size, int _element_count)
{
    element_size = _element_size;
    element_count = _element_count;
    first_index = last_index = 0;
    cur_pos_in_segment = -1;
    current = first = last = nullptr;
    _error = false;
}

List::~List()
{
    first_index = last_index = -1;
    Segment *cur = first;
    while (cur)
    {
        Heap::free_mem(cur->data);
        Segment *next = cur->next;
        Heap::free_mem(cur);
        cur = next;
    }
}

void List::add(const void *data)
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
    memcpy(last->data + offset * element_size, data, element_size);
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
    // void *temp = Heap::get_mem(element_size);
    // memcpy(temp, cur->data + offset * element_size, element_size);
    // return temp;
}


void List::take_first(void *store)
{
    if (first_index == -1 || first_index == last_index)
    {
        _error = true;
        return;
    }
    _error = false;
    if (store != nullptr)
        memcpy(store, first->data + first_index * element_size, element_size);
    if (++first_index == element_count)
    {
        first_index = 0;
        last_index -= element_count;
        delete_segment(first);
    }
    else if (first_index == last_index)
    {
        delete_segment(first);
        first_index = last_index = 0;
        first = last = nullptr;
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
    if (store != nullptr)
        memcpy(store, last->data + ((last_index - 1) % element_count)* element_size, element_size);
    if (--last_index == first_index)
    {
        delete_segment(first);
        last_index = first_index = 0;
    }
    else if (last_index % element_count == 0)
        delete_segment(last);
}

void List::take(int pos, void *store)
{
    pos += first_index;
    if (first_index == -1 || pos >= last_index)
    {
        _error = true;
        return;
    }
    _error = false;
    Segment *cur;
    char *temp = Heap::get_mem(element_size);
    char *boundary = Heap::get_mem(element_size);
    
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
        
        // delete first segment if empty
        if (++first_index == element_count)
        {
            first_index = 0;
            last_index -= element_count;
            delete_segment(first);
        }
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
        
        // delete last segment if empty
        if (--last_index == first_index)
        {
            delete_segment(first);
            last_index = first_index = 0;
        }
        else if (last_index % element_count == 0)
            delete_segment(last);
    }
    Heap::free_mem(temp);
    Heap::free_mem(boundary);
}

void List::new_segment()
{
    Segment *segment = (Segment*) Heap::get_mem(sizeof(Segment));
    segment->data = Heap::get_mem(element_size * element_count);
    segment->prev = segment->next = nullptr;
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
    Heap::free_mem(seg->data);
    Heap::free_mem(seg);
}

int List::count()
{
    if (first_index == -1)
    {
        _error = true;
        return -1;
    }
    _error = false;
    return last_index - first_index;
}

void List::sort(bool dir) // bubble sort
{
    if (first_index == -1)
    {
        _error = true;
        return;
    }
    _error = false;
    if (first_index >= last_index - 1)
        return;
    
    void *temp = Heap::get_mem(element_size);
    bool not_sorted = true;
    char *last_element = last->data + ((last_index - 1) % element_count) * element_size;
    while(not_sorted)
    {
        not_sorted = false;
        Segment* seg = first;
        char *cur_element = seg->data;
        char *seg_last_element = seg->data + (element_count - 1) * element_size;
        while (cur_element != last_element)
        {
            char *next_element;
            if (cur_element == seg_last_element)
            {
                seg = seg->next;
                seg_last_element = seg->data + (element_count - 1) * element_size;
                next_element = seg->data;
            }
            else
                next_element = cur_element + element_size;
            int comp = compare(cur_element, next_element);
            if ((dir && comp > 0) || (!dir && comp < 0))
            {
                not_sorted = true;
                memcpy(temp, cur_element, element_size);
                memcpy(cur_element, next_element, element_size);
                memcpy(next_element, temp, element_size);
            }
            cur_element = next_element;
        }
    }
    Heap::free_mem(temp);
}

void List::set_cur_position_to_start()
{
    current = first;
    cur_pos_in_segment = first_index;
}

void* List::get_next_element()
{
    if (current == nullptr || cur_pos_in_segment < 0)
        return nullptr;
    
    if (current == last &&
            cur_pos_in_segment + 1 == last_index % element_count)
    {
        current = nullptr;
        cur_pos_in_segment = -1;
        return nullptr;
    }
    
    char *next_element = current->data + cur_pos_in_segment++ * element_size;
    if (next_element < current->data + element_size * element_count)
        return next_element;
    
    current = current->next;
    if (current == nullptr)
    {
        cur_pos_in_segment = -1;
        return nullptr;
    }
    cur_pos_in_segment = 1;
    return current->data;
}
