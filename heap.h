#define SEGMENTSIZE 65536

class Heap
{
public:
    Heap(int _segment_size = SEGMENTSIZE)
    {
        segment_size = _segment_size;
        current = nullptr;
    }
    ~Heap()
    {  delete_segments(); }
    void* get_mem (int size);
    void free_mem (void* mem);
private:
    struct Segment_def
    {
        bool used;
        int size;
        int offset;
        Segment_def *prev;
    };

    struct Segment
    {
        int size;
        void *data;
        Segment *prev;
        Segment_def *last_segment_def;
    };

    int use_segment_def(Segment_def* segment_def, int size);
    void delete_segments();

    int segment_size;

    Segment* current;
};
