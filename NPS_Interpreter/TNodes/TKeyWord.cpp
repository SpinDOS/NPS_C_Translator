
#include "TNode.h"
#include "../Variables/VariableTable.h"

using namespace NPS_Interpreter;

void AddVariableBreakAndContinue()
{
    VariableTable::AddVariable("*break", sizeof(bool));
    VariableTable::AddVariable("*continue", sizeof(bool));
    *(bool*)VariableTable::GetVariableType("*break") = false;
    *(bool*)VariableTable::GetVariableType("*continue") = false;
}

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
        AddVariableBreakAndContinue();
        children->get(0)->Exec();
        while(CheckConditio((TBranch*)children->get(1)) &&
                !(*(bool*)VariableTable::GetVariableType("*break"))){ // break is not true
            *(bool*)VariableTable::GetVariableType("*continue") = false;
            if(children->get(3))
                children->get(3)->Exec(); // TBody for
            children->get(2)->Exec(); // inc, dec ...
        }
        VariableTable::PopVisibilityArea();
    }
    if(strcmp(keyword, "break") == 0){
        *(bool*)VariableTable::GetVariableType("*break") = true;
    }
    if(strcmp(keyword, "continue") == 0){
        *(bool*)VariableTable::GetVariableType("*continue") = true;
    }
    /*if(strcmp(keyword, "return")){
        VariableTable::AddVariable("*return", 1);
        globalParameters->push(children->get(0)->Exec());
    }
    */
}
