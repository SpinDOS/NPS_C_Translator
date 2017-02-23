#include <iostream>
#include "list_adv.h"

using namespace std;

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
    return 0;
}
