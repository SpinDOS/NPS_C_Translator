#ifndef __HEAP_H_INCLUDED__
#define __HEAP_H_INCLUDED__

#define SEGMENTSIZE 64*1024
#define nullptr 0

class Heap
{
public:
    static char* get_mem (unsigned int size);
    static void free_mem (void *mem);
private:
    struct Segment_def
    {
        bool used;
        unsigned int size;
        Segment_def *prev;
    };

    struct Segment
    {
        unsigned long size;
        char *data;
        Segment *prev;
        Segment_def *last_segment_def;
    };

    static void use_segment_def(Segment_def *segment_def, unsigned int size);
    static void create_new_segment(unsigned long size);

    static unsigned long segment_size = SEGMENTSIZE;

    static Segment *current = nullptr;
};

#endif // __HEAP_H_INCLUDED__
