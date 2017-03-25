
#include "../collection_containers/list.h"
#include "../LexemeParsing/LexemeParser.h"
#include "HashTable.h"

struct VariableError
{
    char *name;
    char *description;
};

struct Variable_type
{
    int code;
    char *type;

    Variable_type() : code(0), type(""){}
    Variable_type(int code, char *type) : code(code), type(type) {}
};

class VariableParser
{
public:
    VariableParser(){hash_table = new HashTable;}
    int get_sizeof_type(char *type);
    bool parse(TypeList<LexemeWord> &words, VariableError &error);

private:
    HashTable* hash_table;
    bool contains_in_variable_types(int code, char** type);
};
