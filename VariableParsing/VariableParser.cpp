#include <iostream>
#include "VariableParser.h"

using namespace std;

bool VariableParser::contains_in_variable_types(int code, char** type)
{
    int size = 4;
    Variable_type keywords[size];
    keywords[0] = Variable_type(300, "bool");
    keywords[1] = Variable_type(303, "char");
    keywords[2] = Variable_type(309, "double");
    keywords[3] = Variable_type(315, "int");
    for (int i = 0; i < size; ++i)
    {
        if(keywords[i].code == code)
        {
            *type = keywords[i].type;
            return true;
        }
    }
    return false;
}

bool VariableParser::parse(TypeList<LexemeWord> &words, VariableError &error)
{
    int i = 0, count = words.count();
    char *type, *key, *complex_type;
    char *p = "*";
    bool is_complex_type = false;
    int state = 1;
    Variable* variable = nullptr;
    while(i < count)
    {
        LexemeWord *word = words.getTyped(i);
        int code = word->code;
        switch (code / 100)
        {
            // keyword
            case 3:
                if(contains_in_variable_types(code, &type))
                {
                    switch (state)
                    {
                        case 1:
                            state = 2;
                            break;
                        case 2:
                            error.name = "incorrect variable name (type)";
                            return false;
                        case 3:
                            state = 2;
                            is_complex_type = false;
                            break;
                    }
                }
                break;
            // operation * or ,
            case 2:
                if(code == 218) // *
                {
                    switch (state)
                    {
                        case 2:
                            if(!is_complex_type)
                            {
                                is_complex_type = true;
                                complex_type = copy_string(type); //memory leak
                            }
                            complex_type = str_cat(complex_type, p); //
                            break;
                        case 3:
                            error.name = "after variable can not exist *";
                            return false;
                    }
                }
                else if(code == 242) // ,
                {
                    switch (state)
                    {
                        case 2:
                            error.name = "incorrect variable name (,)";
                            return false;
                        case 3:
                            state = 2;
                            is_complex_type = false;
                            Heap::free_mem(complex_type);
                            break;
                    }
                }
                else
                {
                    switch (state)
                    {
                        case 2:
                            error.name = "incorrect variable name (other)";
                            return false;
                        case 3:
                            state = 3;
                            break;
                    }
                }
                break;
            case 4:
                switch (state)
                {
                    case 1:
                        error.name = "variable is not declared";
                        return false;
                    case 2:
                        key = copy_string(word->start, word->length);
                        variable = hash_table->get(key);
                        if(variable == nullptr)
                        {
                            state = 3;
                            variable = static_cast<Variable*>(Heap::get_mem(sizeof(Variable)));
                            if(is_complex_type)
                                variable->type = copy_string(complex_type);
                            else
                                variable->type = copy_string(type);
                            variable->data = nullptr;
                            cout << "variable : " << key << " " << variable->type << endl;
                            hash_table->put(key, variable);
                        }
                        else
                        {
                            error.name = "variable is already declared";
                            return false;
                        }
                        break;
                    case 3:
                        error.name = "variable type is not declared";
                        return false;
                }
                break;
            case 1:
                break;
            default:
                if(state == 2 && i == count - 1)
                {
                    error.name = "incorrect variable name (or end of file)";
                    return false;
                }
                state = 1;
                break;
        }
        i++;
    }
    return true;
}

int VariableParser::get_sizeof_type(char *type)
{
    if(compare_strings(type, "int")){
        return sizeof(int);
    }
    if(compare_strings(type, "double")){
        return sizeof(double);
    }
    if(compare_strings(type, "bool")){
        return sizeof(bool);
    }
    if(compare_strings(type, "char")){
        return sizeof(char);
    }
    return 0;
}
