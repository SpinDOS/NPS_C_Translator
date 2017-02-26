#include <iostream>
#include "list_adv.h"
#include "hash.h"
#include "string_utils.h"

using namespace std;

class ListOfDouble : protected List
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
    using List::sort;
    using List::error;
protected:
	int compare(const void *a, const void *b)
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
    
    cout << "Stack: ";
    Stack stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    cout << stack.pop() << " ";
    cout << stack.pop() << " ";
    cout << stack.pop() << endl;
    
    cout << "Queue: ";
    Queue queue;
    queue.put(1);
    queue.put(2);
    queue.put(3);
    cout << queue.get() << " ";
    cout << queue.get() << " ";
    cout << queue.get() << endl;
    
    cout << "Dequeue: " << endl;
    Deque deque;
    deque.put(1);
    deque.put(2);
    deque.put(3);
    deque.put(4);
    cout << "  First: " << deque.first() << endl;
    cout << "  Last: " << deque.last() << endl;
    cout << "  First: " << deque.first() << endl;
    cout << "  Last: " << deque.last() << endl;
    
    cout << "List: " << endl;
    ListOfDouble list;
    list.add(3);
    list.add(1);
    list.add(2);
    cout << "  Unsorted: ";
    cout << list.getDouble(0) << " ";
    cout << list.getDouble(1) << " ";
    cout << list.getDouble(2) << endl;
    list.sort();
    cout << "  Sorted: ";
    cout << list.getDouble(0) << " ";
    cout << list.getDouble(1) << " ";
    cout << list.getDouble(2) << endl;
    
    cout << "Dictionary: " << endl;
    Diction diction;
    Article *article = diction.auto_create("abc");
    article->description = copy_string("Description for abc");
    article = diction.auto_create("bcd");
    article->description = copy_string("Description for bcd");
    article = diction.auto_create("abd");
    article->description = copy_string("Description for abd");
    article = diction.find("abc");
    cout << "  abc: " << article->description << endl;
    cout << "  abd: " << diction.find("abd")->description << endl;
    cout << "  bcd: " << diction.find("bcd")->description << endl;
    cout << "  Deleting abc.." << endl;
    diction.del("abc");
    cout << "  abc: " << (diction.find("abc")? "Found" : "Not found") << endl;
    
    return 0;
}
