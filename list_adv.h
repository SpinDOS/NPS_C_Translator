#ifndef __LIST_ADV_H_INCLUDED__
#define __LIST_ADV_H_INCLUDED__

#include "list.h"
class Stack : List
{
public:
	Stack() : List(sizeof(double)) { };

	void   push(double x) { add(&x); };
	double pop() { double res; take_last(&res); return res; };
};

class Queue : List
{
public:
	Queue() : List(sizeof(double)) { };

	void   put(double x) { add(&x); };
	double get() { double res; take_first(&res); return res; };
};

class Deque : List
{
public:
    Deque() : List(sizeof(double)) { };

	void   put(double x) { add(&x); };
	double first() { double res; take_first(&res); return res; };
	double last()  { double res; take_last(&res); return res; };

};

#endif // __LIST_ADV_H_INCLUDED__
