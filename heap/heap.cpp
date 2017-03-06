#include "heap.h"
Heap::Segment *Heap::current = nullptr;
int Heap::segment_size = -1;

void* Heap::get_mem(int size)
{
    if (segment_size == -1)
        CreateHeap();
    Segment *cur_segment = current;
    // find free segment
    while (cur_segment)
    {
        if (cur_segment->size >= size)
        {
            // find free segment_def
            Segment_def *cur_def = cur_segment->last_segment_def;
            char *cur_def_start = cur_segment->data + cur_segment->size;
            while (cur_def)
            {
                cur_def_start -= cur_def->size;
                if (!cur_def->used && cur_def->size >= size)
                {
                    use_segment_def(cur_def, size);
                    return cur_def_start;
                }
                cur_def = cur_def->prev;
            }
        }
        cur_segment = cur_segment->prev;
    }
    create_new_segment(size);
    return current->data;
}

void Heap::free_mem (void *mem)
{
    char *chmem = (char*) mem;
    // find segment containing this mem
    Segment *cur_segment = current, *next_segment = nullptr;
    while (cur_segment)
    {
        if (chmem >= cur_segment->data &&
            chmem < cur_segment->data + cur_segment->size)
            break;
        next_segment = cur_segment;
        cur_segment = cur_segment->prev;
    }
    if (!cur_segment)
        return;

    // find segment_def of this mem
    Segment_def *cur_def = cur_segment->last_segment_def, *next_def = nullptr;
    char *cur_def_start = cur_segment->data + cur_segment->size - cur_def->size;
    while (chmem < cur_def_start)
    {
        next_def = cur_def;
        cur_def = cur_def->prev;
        cur_def_start -= cur_def->size;
    }
    if (chmem != cur_def_start)
        return;

    // union near-placed segment-def's
    Segment_def *prev_def = cur_def->prev;
    if (prev_def && !prev_def->used)
    {
        cur_def->size += prev_def->size;
        cur_def->prev = prev_def->prev;
        delete prev_def;
    }
    if (next_def && !next_def->used)
    {
        next_def->size += cur_def->size;
        next_def->prev = cur_def->prev;
        delete cur_def;
    }
    else
    {
        cur_def->used = false;
    }

    // delete empty segment
    if (cur_segment->last_segment_def->size == cur_segment->size)
    {
        if (next_segment)
            next_segment->prev = cur_segment->prev;
        else
            current = cur_segment->prev;
        delete cur_segment->last_segment_def;
        delete [] cur_segment->data;
        delete cur_segment;
    }
}


void Heap::use_segment_def(Segment_def* segment_def, int size)
{
    if (size != segment_def->size)
    {
        Segment_def* new_segment_def = new Segment_def;
        new_segment_def->size = size;
        segment_def->size -= size;

        new_segment_def->prev = segment_def->prev;
        segment_def->prev = new_segment_def;

        segment_def = new_segment_def;
    }
    segment_def->used = true;
}

void Heap::create_new_segment(int size)
{
    // create segment
    int new_segment_size = size > segment_size? size : segment_size;
    Segment* segment = new Segment;
    segment->data = new char[new_segment_size];
    segment->size = new_segment_size;
    segment->prev = current;
    current = segment;

    // create segment_def's of the segment
    Segment_def* first_def = new Segment_def;
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
        second_def->prev = first_def;
        second_def->size = new_segment_size - size;
        second_def->used = false;
        segment->last_segment_def = second_def;
    }
}

void Heap::delete_segments()
{
    while (current)
    {
        Segment *prev_seg = current->prev;
        Segment_def *cur_def = current->last_segment_def;
        while (cur_def)
        {
            Segment_def *prev_def = cur_def->prev;
            delete cur_def;
            cur_def = prev_def;
        }
        delete [] current->data;
        delete current;
        current = prev_seg;
    }
}

