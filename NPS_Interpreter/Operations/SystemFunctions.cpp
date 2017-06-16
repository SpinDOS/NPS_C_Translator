//
// Created by Alexander on 05-May-17.
//

#include "math.h"
#include "iostream"
#include "OperationManager.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;
using namespace std;

int get_int()
{
    char *param = GlobalParameters()->takeLast();
    int result;
    memcpy(&result, param, sizeof(int));
    Heap::free_mem(param);
    return result;
}

void write_pointer(void *p)
{
    char *result = Heap::get_mem(sizeof(char*));
    *reinterpret_cast<void**>(result) = p;
    GlobalParameters()->add(result);
}

struct SystemNew : TList
{
    char* Exec() final
    {
        write_pointer(Heap::get_mem(get_int()));
        return nullptr;
    }
};

struct SystemDelete : TList
{
    char* Exec() final
    {
        char *param = GlobalParameters()->takeLast();
        Heap::free_mem(*reinterpret_cast<void**>(param));
        Heap::free_mem(param);
        return nullptr;
    }
};

// =========================================================================

void write_int(int i);

struct SystemOutput : TList
{
    char* Exec() final
    {
        char *param = GlobalParameters()->takeLast();
        cout << *reinterpret_cast<char**>(param) << flush;
        Heap::free_mem(param);
        return nullptr;
    }
};

struct SystemOutputDouble : TList
{
    char* Exec() final
    {
        char *param = GlobalParameters()->takeLast();
        cout << *reinterpret_cast<double*>(param) << flush;
        Heap::free_mem(param);
        return nullptr;
    }
};


struct SystemInput : TList
{
    char* Exec() final
    {
        char temp[1024];
        if (!fgets(temp, 1024, stdin))
            write_pointer(nullptr);
        int length = strlen(temp);
        if (temp[length - 1] == '\n')
            temp[length - 1] = '\0';
        write_pointer(copy_string(temp));
        return nullptr;
    }
};

struct SystemInputArray : TList
{
    char* Exec() final
    {
        int length = get_int();
        
        char *param1 = GlobalParameters()->takeLast();
        char *arr = *reinterpret_cast<char**>(param1);
        Heap::free_mem(param1);
    
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
        int i2 = get_int(), i1 = get_int();
        write_int(i1 < i2? i1 : i2);
        return nullptr;
    }
};

struct SystemMax: TList
{
    char* Exec() final
    {
        int i2 = get_int(), i1 = get_int();
        write_int(i1 > i2? i1 : i2);
        return nullptr;
    }
};

// =========================================================================

double get_double()
{
    char *param = GlobalParameters()->takeLast();
    double result;
    memcpy(&result, param, sizeof(double));
    Heap::free_mem(param);
    return result;
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
        write_double(sin(get_double()));
        return nullptr;
    }
};

struct SystemCos: TList
{
    char* Exec() final
    {
        write_double(cos(get_double()));
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
    add_to_var_table("new#0", new SystemNew);
    add_to_var_table("delete#0", new SystemDelete);
    add_to_var_table("output#0", new SystemOutput);
    add_to_var_table("output#1", new SystemOutputDouble);
    add_to_var_table("input#0", new SystemInput);
    add_to_var_table("input#1", new SystemInputArray);
    add_to_var_table("min#0", new SystemMin);
    add_to_var_table("max#0", new SystemMax);
    add_to_var_table("sin#0", new SystemSin);
    add_to_var_table("cos#0", new SystemCos);
    
}