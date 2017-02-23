#include <iostream>
#include "list_adv.h"

using namespace std;

class ListOfDouble : List
{
public:
	ListOfDouble() : List(sizeof(double)) { };
	void add (double x) { List::add(&x); };
	double getDouble(int pos)
	{
	    double *d = (double*) get(pos);
	    double result = *d;
	    Heap::free_mem(d);
	    return result;
    };
    void sort(bool dir = true) {List::sort(dir);}
protected:
	int compare(void *a, void *b)
	{
	    double d1 = *(double*)a;
	    double d2 = *(double*)b;
	    if (d1 > d2)
            return 1;
        else if (d1 < d2)
            return -1;
        else
            return 0;
	}
};

int main()
{
    Heap::CreateHeap();
    cout << "Hello world!" << endl;
    Stack s;
    s.push(1);
    s.push(2);
    s.push(3);
    cout << "Stack: ";
    cout << s.pop() << " ";
    cout << s.pop() << " ";
    cout << s.pop() << endl;
    Queue q;
    q.put(1);
    q.put(2);
    q.put(3);
    cout << "Queue: ";
    cout << q.get() << " ";
    cout << q.get() << " ";
    cout << q.get() << endl;
    Deque d;
    d.put(1);
    d.put(2);
    d.put(3);
    d.put(4);
    cout << "Dequeue: " << endl;
    cout << "  First: " << d.first() << endl;
    cout << "  Last: " << d.last() << endl;
    cout << "  First: " << d.first() << endl;
    cout << "  Last: " << d.last() << endl;

    ListOfDouble l;
    l.add(3);
    l.add(1);
    l.add(2);
    cout << "List: " << endl;
    cout << "  Unsorted: ";
    cout << l.getDouble(0) << " ";
    cout << l.getDouble(1) << " ";
    cout << l.getDouble(2) << endl;
    l.sort();
    cout << "  Sorted: ";
    cout << l.getDouble(0) << " ";
    cout << l.getDouble(1) << " ";
    cout << l.getDouble(2) << endl;
    return 0;
}
