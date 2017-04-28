#include <iostream>
#include "Deserialization/TreeParser.h"
#include "Functions/ParameterManager.h"
#include "Variables/VariableTable.h"

using namespace std;

using namespace NPS_Interpreter;

TSimpleLinkedList<TNode>* ParseTree(char* path){
    TreeParser *parser = new TreeParser();
    TSimpleLinkedList<TNode>* instructions = parser->Deserialize(path);
    delete parser;
    return instructions;
}


int main(int argc, char *argv[]) {
    TSimpleLinkedList<TNode>* instructions = ParseTree(argv[1]);
    for (int i = 0; i < instructions->count(); ++i) {
        instructions->get(i)->Exec();
    }
    Func* main = (Func *) VariableTable::GetVariableType("main");
    main->Exec();
    return 0;
}