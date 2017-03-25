
#include "../collection_containers/list.h"
#include "../LexemeParsing/LexemeParser.h"
#include "HashTable.h"

struct VariableError
{
    char *name;
    char *description;
};

class VariableParser
{
public:
    VariableParser(){hash_table = new HashTable;}
    int get_sizeof_type(char *type);
    bool parse(TypeList<LexemeWord> &words, VariableError &error);

private:
    HashTable* hash_table;
};
