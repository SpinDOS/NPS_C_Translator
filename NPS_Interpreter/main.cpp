#include <iostream>
#include "Deserialization/TreeParser.h"
#include "Functions/FunctionManager.h"

using namespace std;

using namespace NPS_Interpreter;


int main(int argc, char *argv[]) {
    char *path = argv[1];
    TreeParser *parser = new TreeParser();
    TSimpleLinkedList<TNode>* instructions = parser->Deserialize(path);
    delete parser;
    for (int i = 0; i < instructions->count(); ++i) {
        instructions->get(i)->Exec();
    }
    Func* main = FunctionManager::functions_table->get("main");
//    pushparams(argc);
    
    main->Exec();
    return 0;
}