
#include "../collection_containers/list.h"
#include "../LexemeParsing/LexemeParser.h"
#include "HashTable.h"

class VariableParser
{
public:
    VariableParser(){hash_table = new HashTable;}
    int get_sizeof_type(char *type);
    void parse(TypeList<LexemeWord> &words);

private:
    HashTable* hash_table;
};
