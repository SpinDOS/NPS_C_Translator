#include <iostream>
#include "collection_containers/list_adv.h"
#include "collection_containers/hash.h"
#include "utils/string_utils.h"

using namespace std;

class ListOfDouble : protected List
{
public:
	ListOfDouble() : List(sizeof(double)) { };
	void add (double x) { List::add(&x); };
	double getDouble(int pos)
	{
	    return *(double*) get(pos);
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
    return 0;
}
