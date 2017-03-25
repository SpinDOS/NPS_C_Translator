#include <iostream>
#include "VariableParser.h"

using namespace std;

struct Variable_type
{
    int code;
    char *type;

    Variable_type() : code(0), type(""){}
    Variable_type(int code, char *type) : code(code), type(type) {}
};

void VariableParser::parse(TypeList<LexemeWord> &words)
{
    int keywords_size = 4;
    Variable_type keywords[keywords_size];
    keywords[0] = Variable_type(300, "bool");
    keywords[1] = Variable_type(303, "char");
    keywords[2] = Variable_type(309, "double");
    keywords[3] = Variable_type(315, "int");
    int i = 0;
    char* type;
    int words_count = words.count();
    while (i < words_count)
    {
        LexemeWord *word = static_cast<LexemeWord*>(words.get(i));
        int code = word->code;
        bool contain = false;
        for (int j = 0; j < keywords_size && !contain; ++j)
        {
            Variable_type variable_type = keywords[j];
            if(variable_type.code == code)
            {
                contain = true;
                type = copy_string(variable_type.type);
            }
        }
        if(contain && i + 1 < words_count)
        {
            LexemeWord *variable_word = static_cast<LexemeWord*>(words.get(i + 1));
            if(variable_word->code / 100 == 4) // 4** - variables
            {
                char* key = copy_string(variable_word->start, variable_word->length);
                Variable* variable = nullptr;
                variable = hash_table->get(key);
                if(variable == nullptr)
                {
                    variable = static_cast<Variable*>(Heap::get_mem(sizeof(Variable)));
                    variable->type = type;
                    variable->data = nullptr;
                    cout << "variable " << key << " type - " << variable->type << " size - " << get_sizeof_type(variable->type) << endl;
                    hash_table->put(key, variable);
                }
                else
                {
                    cout << "variable " << key << " is already declared" << endl;
                }
                i++;
            }
            else
            {
                cout << "error with variable declaration " << endl;
            }
        }
        i++;
    }
}

int VariableParser::get_sizeof_type(char *type)
{
    if(compare_strings(type, "int")){
        return sizeof(int);
    }
    if(compare_strings(type, "double")){
        return sizeof(double);
    }
    if(compare_strings(type, "float")){
        return sizeof(float);
    }
    if(compare_strings(type, "char")){
        return sizeof(char);
    }
    return 0;
}
