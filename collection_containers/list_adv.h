#ifndef __LIST_ADV_H_INCLUDED__
#define __LIST_ADV_H_INCLUDED__

#include "list.h"
template <typename T>
class Stack : protected List
{
public:
	Stack() : List(sizeof(T)) { };

	void   push(const T &data) { add(&data); };
	T pop() { T res; take_last(&res); return res; };

    using List::count;
	using List::error;
};

template <typename T>
class Queue : protected List
{
public:
	Queue() : List(sizeof(T)) { };

	void   put(T &x) { add(&x); };
	T get() { T res; take_first(&res); return res; };

    using List::count;
    using List::error;
};

template <typename T>
class Deque : protected List
{
public:
    Deque() : List(sizeof(T)) { };

	void   put(T x) { add(&x); };
	T first() { T res; take_first(&res); return res; };
	T last()  { T res; take_last(&res); return res; };

    using List::count;
    using List::error;
    
};

#endif // __LIST_ADV_H_INCLUDED__
