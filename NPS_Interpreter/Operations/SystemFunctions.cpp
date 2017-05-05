//
// Created by Alexander on 05-May-17.
//

#include "math.h"
#include "iostream"
#include "OperationManager.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;
using namespace std;

struct SystemOutput : TList
{
    ReturnResult Exec() final
    {
        ReturnResult param = GlobalParameters()->take_first();
        cout << *reinterpret_cast<char**>(param.data);
        param.FreeIfNeed();
        return ReturnResult();
    }
};

struct SystemInput : TList
{
    ReturnResult Exec() final
    {
        string str;
        cin >> str;
        ReturnResult result;
        result.need_to_free_mem = true;
        result.data = (char *) Heap::get_mem(sizeof(char*));
        *reinterpret_cast<char**>(result.data) = copy_string(str.c_str());
        return result;
    }
};

// =========================================================================

void get_two_ints(int *i1, int *i2)
{
    ReturnResult param1 = GlobalParameters()->take_first();
    ReturnResult param2 = GlobalParameters()->take_first();
    memcpy(i1, param1.data, sizeof(int));
    memcpy(i2, param2.data, sizeof(int));
    param1.FreeIfNeed();
    param2.FreeIfNeed();
}

void write_int(int i)
{
    ReturnResult result;
    result.data = static_cast<char*>(Heap::get_mem(sizeof(int)));
    memcpy(result.data, &i, sizeof(int));
    result.need_to_free_mem = true;
    GlobalParameters()->addTyped(result);
}

struct SystemMin: TList
{
    ReturnResult Exec() final
    {
        int i1, i2;
        get_two_ints(&i1, &i2);
        write_int(i1 < i2? i1 : i2);
        return ReturnResult();
    }
};

struct SystemMax: TList
{
    ReturnResult Exec() final
    {
        int i1, i2;
        get_two_ints(&i1, &i2);
        write_int(i1 > i2? i1 : i2);
        return ReturnResult();
    }
};

// =========================================================================

void get_double(double *d)
{
    ReturnResult param = GlobalParameters()->take_first();
    memcpy(d, param.data, sizeof(double));
    param.FreeIfNeed();
}

void write_double(double d)
{
    ReturnResult result;
    result.data = static_cast<char*>(Heap::get_mem(sizeof(double)));
    memcpy(result.data, &d, sizeof(double));
    result.need_to_free_mem = true;
    GlobalParameters()->addTyped(result);
}

struct SystemSin: TList
{
    ReturnResult Exec() final
    {
        double d;
        get_double(&d);
        write_double(sin(d));
        return ReturnResult();
    }
};

struct SystemCos: TList
{
    ReturnResult Exec() final
    {
        double d;
        get_double(&d);
        write_double(cos(d));
        return ReturnResult();
    }
};

// =========================================================================

void add_to_var_table(const char *name, TList *implementation)
{
    VariableTable::AddVariable(name, sizeof(TList*));
    char *in_table = VariableTable::GetVariableData(name);
    memcpy(in_table, &implementation, sizeof(TList*));
}

void OperationManager::Init()
{
    // add system functions here
    // sin, cos, min, max, input, output
    add_to_var_table("output#0", new SystemOutput);
    add_to_var_table("input#0", new SystemInput);
    add_to_var_table("min#0", new SystemMin);
    add_to_var_table("max#0", new SystemMax);
    add_to_var_table("sin#0", new SystemSin);
    add_to_var_table("cos#0", new SystemCos);
    
}