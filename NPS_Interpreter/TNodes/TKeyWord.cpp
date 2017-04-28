
#include "TNode.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;

bool CheckConditio(TBranch* tbranch){
    return *(bool*)tbranch->Exec();
}

char* TKeyword::Exec() {
    if(strcmp(keyword, "if") == 0){
        if(CheckConditio((TBranch*)children->get(0))){
            return children->get(1)->Exec();
        }
        else if(children->get(2) != nullptr){
            return children->get(2)->Exec();
        }
    }
    if(strcmp(keyword, "for") == 0){
        VariableTable::PushVisibilityArea();
        children->get(0)->Exec();
        while(CheckConditio((TBranch*)children->get(1))
                /*&& VariableTable::GetVariableType("*break") == nullptr*/){
            children->get(3)->Exec(); // TBody for
            children->get(2)->Exec(); // inc, dec ...
        }
        VariableTable::PopVisibilityArea();
    }
    /*if(strcmp(keyword, "return")){
        VariableTable::AddVariable("*return", 1);
        globalParameters->push(children->get(0)->Exec());
    }
    if(strcmp(keyword, "break")){
        VariableTable::AddVariable("*break", 1);
    }
    if(strcmp(keyword, "continue")){
        VariableTable::AddVariable("*continue", 1);
    }*/
}
