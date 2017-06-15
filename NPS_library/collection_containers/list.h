#ifndef __LIST_H_INCLUDED__
#define __LIST_H_INCLUDED__

#include "../heap/heap.h"

class BaseList
{
public:
	BaseList(int _element_size, int _element_count = 64)
    { element_size = _element_size; element_count = _element_count; }
	virtual ~BaseList();

	void* get(int pos) const;
	void* get_first() const;
	void* get_last() const;
	void add(const void *data);

	// returns and deletes elements
	void take_first(void *store);
	void take_last(void *store);
	void take(int pos, void *store);

    int count() const;

private:
	struct Segment
	{
		char *data;
		Segment *prev;
		Segment *next;
	};
	Segment *first = nullptr;
	Segment *last = nullptr;
	int first_index = 0;
	int last_index = 0;
	
	Segment *current = nullptr;

	int element_size;
	int element_count;

	void create_new_segment();
	void increase_first_index();
    void decrease_last_index();
};

template <typename T> class List : public BaseList
{
public:
    List() : BaseList(sizeof(T)) {}
    List(int _element_count) : BaseList(sizeof(T), _element_count) {}
	T* Get(int pos) const {return reinterpret_cast<T*>(BaseList::get(pos));}
    T* GetFirst() const {return reinterpret_cast<T*>(BaseList::get_first());}
    T* GetLast() const {return reinterpret_cast<T*>(BaseList::get_last());}
	void Add(const T &data) {BaseList::add(&data);}
	T TakeFirst() {T t; BaseList::take_first(&t); return t;}
	T TakeLast() {T t; BaseList::take_last(&t); return t;}
	T Take(int index) {T t; BaseList::take(index, &t); return t;}
};

#endif // __LIST_H_INCLUDED__
