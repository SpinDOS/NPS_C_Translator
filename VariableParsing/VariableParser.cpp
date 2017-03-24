#include <iostream>
#include <cstring>
#include "VariableParser.h"

using namespace std;

void VariableParser::parse(TypeList<LexemeWord> words)
{
    int variable_type_array_size = 4;
    int variable_type_keywords[] = {300, 303, 309, 315}; // bool char double int
    char *variable_types[] = {"bool", "char", "double", "int"};
    int i = 0;
    char* type;
    int words_count = words.count();
    while (i < words_count)
    {
        LexemeWord *word = static_cast<LexemeWord*>(words.get(i));
        int code = word->code;
        bool contain = false;
        for (int j = 0; j < variable_type_array_size && !contain; ++j)
            if(variable_type_keywords[j] == code)
            {
                contain = true;
                type = variable_types[j];
            }
        if(contain && i + 1 < words_count)
        {
            // 400 - это variable
            LexemeWord *variable_word = static_cast<LexemeWord*>(words.get(i + 1));
            if(variable_word->code / 100 == 4)
            {
                char* key = copy_string(variable_word->start, variable_word->length);
                Variable* variable = nullptr;
                variable = hash_table->get(key);
                if(variable == nullptr)
                {
                    variable = (Variable*)Heap::get_mem(sizeof(Variable));
                    variable->type = type;
                    variable->name = key;
                    cout << "variable " << variable->type << " " << variable->name << endl;
                    hash_table->put(key, variable);
                }
                else
                {
                    cout << "variable is already declared" << endl;
                }
                i++;
            }
            else
            {
                cout << "error with variable declaration" << endl;
            }
        }
        i++;
    }
}

int VariableParser::get_sizeof_type(char *type)
{
    if(!strcmp(type, "int")){
        return sizeof(int);
    }
    if(!strcmp(type, "double")){
        return sizeof(double);
    }
    if(!strcmp(type, "float")){
        return sizeof(float);
    }
    if(!strcmp(type, "char")){
        return sizeof(char);
    }
    return 0;
}
