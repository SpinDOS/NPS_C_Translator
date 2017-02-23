#include "heap.h"
Heap::Segment* Heap::current = nullptr;
int Heap::segment_size = -1;

void* Heap::get_mem(int size)
{
    if (segment_size == -1)
        CreateHeap();
    Segment *cur_segment = current;
    while (cur_segment)
    {
        if (cur_segment->size >= size)
        {
            Segment_def *cur_def = cur_segment->last_segment_def;
            while (cur_def)
            {
                if (!cur_def->used && cur_def->size >= size)
                    return (char*) cur_segment->data + use_segment_def(cur_def, size);

                cur_def = cur_def->prev;
            }
        }
        cur_segment = cur_segment->prev;
    }

    int new_segment_size = size > segment_size? size : segment_size;
    Segment* segment = new Segment;
    segment->data = new char[new_segment_size];
    segment->prev = current;
    current = segment;

    Segment_def* first_def = new Segment_def;
    first_def->offset = 0;
    first_def->prev = nullptr;
    first_def->size = size;
    first_def->used = true;
    if (size == new_segment_size)
    {
        segment->last_segment_def = first_def;
    }
    else
    {
        Segment_def* second_def = new Segment_def;
        second_def->offset = size;
        second_def->prev = first_def;
        second_def->size = new_segment_size - size;
        second_def->used = false;
        segment->last_segment_def = second_def;
    }
    return segment->data;
}

void Heap::free_mem (void* mem)
{
    char *chmem = (char*) mem;
    Segment *cur_segment = current, *next_segment = nullptr;
    while (cur_segment)
    {
        if (chmem >= (char*) cur_segment->data &&
            chmem < (char*) cur_segment->data + cur_segment->size)
            break;
        next_segment = cur_segment;
        cur_segment = cur_segment->prev;
    }
    if (!cur_segment)
        return;

    char *segment_start = (char*) cur_segment->data;
    Segment_def *cur_def = cur_segment->last_segment_def, *next_def = nullptr;
    while (chmem < segment_start + cur_def->offset)
    {
        next_def = cur_def;
        cur_def = cur_def->prev;
    }
    if (chmem != segment_start + cur_def->offset)
        return;

    Segment_def *prev_def = cur_def->prev;
    if (prev_def && !prev_def->used)
    {
        cur_def->offset = prev_def->offset;
        cur_def->size += prev_def->size;
        cur_def->prev = prev_def->prev;
        delete prev_def;
    }
    if (next_def && !next_def->used)
    {
        next_def->offset = cur_def->offset;
        next_def->size += cur_def->size;
        next_def->prev = cur_def->prev;
        delete cur_def;
    }
    else
    {
        cur_def->used = false;
    }

    if (cur_def->size == cur_segment->size)
    {
        if (next_segment)
            next_segment->prev = cur_segment->prev;
        delete cur_def;
        delete [] (char*) cur_segment->data;
        delete cur_segment;
    }
}


int Heap::use_segment_def(Segment_def* segment_def, int size)
{
    if (size != segment_def->size)
    {
        Segment_def* new_segment_def = new Segment_def;
        new_segment_def->size = size;
        segment_def->size -= size;

        new_segment_def->offset = segment_def->offset;
        segment_def->offset += size;

        new_segment_def->prev = segment_def->prev;
        segment_def->prev = new_segment_def;

        segment_def = new_segment_def;
    }
    segment_def->used = true;
    return segment_def->offset;
}

void Heap::delete_segments()
{
    while (current)
    {
        Segment *prev = current->prev;
        Segment_def *cur_def = current->last_segment_def;
        while (cur_def)
        {
            Segment_def *prev = cur_def->prev;
            delete cur_def;
            cur_def = prev;
        }
        delete [] (char*) current->data;
        delete current;
        current = prev;
    }
}

