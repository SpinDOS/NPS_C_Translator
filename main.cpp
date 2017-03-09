#include <iostream>
#include "collection_containers/list_adv.h"
#include "collection_containers/hash.h"
#include "utils/string_utils.h"
#include <string>

using namespace std;



class A
{
public:
    ~A(){
        cout << "Distr";
    }
};

A fd(){
    A a;
    return a;
}

int main()
{
    int c = 1;
    A b = fd();
    cout << c;
    for (int i = 0; i < 100000; i++)
        cout << "ds" << i << endl;
    return 0;
}