#include <iostream>
#include "Deserialization/TreeParser.h"
#include "Functions/ParameterManager.h"
#include "Variables/VariableTable.h"
#include "TNodes/TNode.h"

using namespace std;

using namespace NPS_Interpreter;

TSimpleLinkedList<TNode>* ParseTree(char* path){
    TreeParser parser;
    return parser.Deserialize(path);
}


int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        cout << "Usage: NPS_Interpreter object_file" << endl;
        return 1;
    }
    TSimpleLinkedList<TNode>* instructions = ParseTree(argv[1]);
    if (instructions == nullptr)
    {
        cout << "Invalid object file " << argv[1] << endl;
        return 1;
    }
    for (int i = 0; i < instructions->count(); ++i) {
        instructions->get(i)->Exec();
    }
    FuncContainer* main = (FuncContainer *) VariableTable::GetVariableData("main#0");
    main->instructions->Exec();
    return 0;
}