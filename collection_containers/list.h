#ifndef __LIST_H_INCLUDED__
#define __LIST_H_INCLUDED__

#include "../heap/heap.h"
#define LISTSIZE 64

class List
{
public:
	List(int _element_size, int _element_count = LISTSIZE);
	virtual ~List();

	void* get(int pos);
	void add(const void *data);

	// returns and deletes elements
	void take_first(void *store);
	void take_last(void *store);
	void take(int pos, void *store);
	void sort(bool dir = true);

    int count();
    bool error() { return _error; } // true if error in last operation

protected:
    virtual int compare (const void *a, const void *b) { return 0; };

private:
	struct Segment
	{
		char *data;
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

template <typename T>
class TypeList : public List
{
public:
	TypeList(int _element_count = LISTSIZE) : List(sizeof(T), _element_count){}
	T* getTyped(int pos){return static_cast<T*>(List::get(pos));}
	void addTyped(const T &data){List::add(&data);}
};

#endif // __LIST_H_INCLUDED__
