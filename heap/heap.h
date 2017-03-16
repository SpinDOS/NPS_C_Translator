#ifndef __HEAP_H_INCLUDED__
#define __HEAP_H_INCLUDED__

#define SEGMENTSIZE 65536
#define nullptr 0

class Heap
{
public:
    // NEVER USE IT WITH GLOBAL VARIABLES
    static void CreateHeap(int _segment_size = SEGMENTSIZE)
    {
        delete_segments();
        segment_size = _segment_size;
    }
    static void* get_mem (int size);
    static void free_mem (void *mem);
private:
    struct Segment_def
    {
        bool used;
        int size;
        Segment_def *prev;
    };

    struct Segment
    {
        int size;
        char *data;
        Segment *prev;
        Segment_def *last_segment_def;
    };

    static void use_segment_def(Segment_def *segment_def, int size);
    static void create_new_segment(int size);
    static void delete_segments();

    static int segment_size;

    static Segment *current;
};

#endif // __HEAP_H_INCLUDED__
