#ifndef __HEAP_H_INCLUDED__
#define __HEAP_H_INCLUDED__

#define SEGMENTSIZE 65536
#define nullptr 0

class Heap
{
public:
    // NEVER USE IT WITH GLOBAL VARIABLES
    static void CreateHeap(unsigned long _segment_size = SEGMENTSIZE)
    {
        delete_segments();
        segment_size = _segment_size;
    }
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
    static void delete_segments();

    static unsigned long segment_size;

    static Segment *current;
};

#endif // __HEAP_H_INCLUDED__
