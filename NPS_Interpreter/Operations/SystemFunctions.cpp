//
// Created by Alexander on 05-May-17.
//

#include "math.h"
#include "iostream"
#include "OperationManager.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;
using namespace std;

void write_int(int i);

struct SystemOutput : TList
{
    char* Exec() final
    {
        char *param = GlobalParameters()->takeFirst();
        cout << *reinterpret_cast<char**>(param) << flush;
        Heap::free_mem(param);
        return nullptr;
    }
};

struct SystemOutputDouble : TList
{
    char* Exec() final
    {
        char *param = GlobalParameters()->takeFirst();
        cout << *reinterpret_cast<double*>(param) << flush;
        Heap::free_mem(param);
        return nullptr;
    }
};


struct SystemInput : TList
{
    char* Exec() final
    {
        string str;
        cin >> str;
        char *result = Heap::get_mem(sizeof(char*));
        *reinterpret_cast<char**>(result) = copy_string(str.c_str());
        GlobalParameters()->add(result);
        return nullptr;
    }
};

struct SystemInputArray : TList
{
    char* Exec() final
    {
        char *param1 = GlobalParameters()->takeFirst();
        char *param2 = GlobalParameters()->takeFirst();
        
        char *arr = *reinterpret_cast<char**>(param1);
        int length;
        memcpy(&length, param2, sizeof(int));
        
        Heap::free_mem(param1);
        Heap::free_mem(param2);
        
        if (fgets(arr, length, stdin) == nullptr)
            length = -1;
        else
        {
            length = strlen(arr);
            if (arr[length - 1] == '\n')
                arr[--length] = '\0';
        }
        write_int(length);
        return nullptr;
    }
};

// =========================================================================

void get_two_ints(int *i1, int *i2)
{
    char *param1 = GlobalParameters()->takeFirst();
    char *param2 = GlobalParameters()->takeFirst();
    memcpy(i1, param1, sizeof(int));
    memcpy(i2, param2, sizeof(int));
    Heap::free_mem(param1);
    Heap::free_mem(param2);
}

void write_int(int i)
{
    char *result = Heap::get_mem(sizeof(int));
    memcpy(result, &i, sizeof(int));
    GlobalParameters()->add(result);
}

struct SystemMin: TList
{
    char* Exec() final
    {
        int i1, i2;
        get_two_ints(&i1, &i2);
        write_int(i1 < i2? i1 : i2);
        return nullptr;
    }
};

struct SystemMax: TList
{
    char* Exec() final
    {
        int i1, i2;
        get_two_ints(&i1, &i2);
        write_int(i1 > i2? i1 : i2);
        return nullptr;
    }
};

// =========================================================================

void get_double(double *d)
{
    char *param = GlobalParameters()->takeFirst();
    memcpy(d, param, sizeof(double));
    Heap::free_mem(param);
}

void write_double(double d)
{
    char *result = Heap::get_mem(sizeof(double));
    memcpy(result, &d, sizeof(double));
    GlobalParameters()->add(result);
}

struct SystemSin: TList
{
    char* Exec() final
    {
        double d;
        get_double(&d);
        write_double(sin(d));
        return nullptr;
    }
};

struct SystemCos: TList
{
    char* Exec() final
    {
        double d;
        get_double(&d);
        write_double(cos(d));
        return nullptr;
    }
};

// =========================================================================

void add_to_var_table(const char *name, TList *implementation)
{
    char *p = reinterpret_cast<char*> (implementation);
    VariableTable::AddVariableWithData(name, p);
}

void OperationManager::Init()
{
    // add system functions here
    // sin, cos, min, max, input, output
    add_to_var_table("output#0", new SystemOutput);
    add_to_var_table("output#1", new SystemOutputDouble);
    add_to_var_table("input#0", new SystemInput);
    add_to_var_table("input#1", new SystemInputArray);
    add_to_var_table("min#0", new SystemMin);
    add_to_var_table("max#0", new SystemMax);
    add_to_var_table("sin#0", new SystemSin);
    add_to_var_table("cos#0", new SystemCos);
    
}