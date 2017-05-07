#include <iostream>
#include "Deserialization/TreeParser.h"
#include "Variables/VariableTable.h"
#include "Operations/OperationManager.h"

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
    OperationManager::Init();
    for (int i = 0; i < instructions->count(); ++i)
        instructions->get(i)->Exec();
    
    TFunction function;
    TVariable *variable = new TVariable;
    variable->name = copy_string("main#0");
    function.children.add(variable);
    VariableTable::AddVariable("break", sizeof(bool));
    VariableTable::AddVariable("continue", sizeof(bool)); // for main list
    char *returnValue = function.Exec();
    return *reinterpret_cast<int*>(returnValue);
}