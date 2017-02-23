#include "heap.h"
#define LISTSIZE 64

class List
{
public:
	List(int _element_size, int _element_count = LISTSIZE);
	~List();

	void* get(int pos);
	void add(void *data);

	// returns and deletes elements
	void take_first(void *store);
	void take_last(void *store);
	void take(int pos, void *store);
	void sort(bool dir = true);

    int count();
    bool error() { return _error; } // true if error in last operation

protected:
    virtual int compare (void *a, void *b) { return 0; };

private:
	struct Segment
	{
		void *data;
		Segment *prev;
		Segment *next;
	};
	Segment *first;
	Segment *last;
	int first_index;
	int last_index;

	int element_size;
	int element_count;
	bool _error;

	void new_segment();
	void delete_segment(Segment *seg);
};

