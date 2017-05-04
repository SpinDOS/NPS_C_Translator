//
// Created by Alexander on 04-May-17.
//

#include "OperationManager.h"

using namespace NPS_Interpreter;

ReturnResult NPS_Interpreter::TSwitchCase::Exec()
{
    return ReturnResult();
}

//bool VariableIsFalse(char* key){
//    return !*(bool*) VariableTable::GetVariableData(key);
//}
//
//void AddVariableBreakAndContinue()
//{
//    VariableTable::AddVariable("*break", sizeof(bool));
//    VariableTable::AddVariable("*continue", sizeof(bool));
//    *(bool*) VariableTable::GetVariableData("*break") = false;
//    *(bool*) VariableTable::GetVariableData("*continue") = false;
//}
//
//bool CheckConditio(TBranch* tbranch){
//    return *(bool*)tbranch->Exec() == true ||
//           *(double*)tbranch->Exec() != 0;
//}
//
//char* TKeyword::Exec() {
//    if(strcmp(keyword, "if") == 0){
//        if(CheckConditio((TBranch*)children.get(0))){
//            return children.get(1)->Exec();
//        }
//        else if(children.get(2) != nullptr){
//            return children.get(2)->Exec();
//        }
//    }
//    if(strcmp(keyword, "for") == 0){
//        VariableTable::PushVisibilityArea();
//        AddVariableBreakAndContinue();
//        children.get(0)->Exec();
//        while(CheckConditio((TBranch*)children.get(1)) &&
//              VariableIsFalse("*break") &&
//              VariableIsFalse("*return")){
//            *(bool*) VariableTable::GetVariableData("*continue") = false;
//            if(children.get(3))
//                children.get(3)->Exec(); // TBody for
//            if(VariableIsFalse("*return"))
//                children.get(2)->Exec(); // inc, dec ...
//        }
//        VariableTable::PopVisibilityArea();
//    }
//    if(strcmp(keyword, "while") == 0){
//        VariableTable::PushVisibilityArea();
//        AddVariableBreakAndContinue();
//        while(CheckConditio((TBranch*)children.get(0)) &&
//              VariableIsFalse("*break") &&
//              VariableIsFalse("*return")){
//            *(bool*) VariableTable::GetVariableData("*continue") = false;
//            if(children.get(1))
//                children.get(1)->Exec();
//        }
//        VariableTable::PopVisibilityArea();
//    }
//    if(strcmp(keyword, "do") == 0){
//        VariableTable::PushVisibilityArea();
//        AddVariableBreakAndContinue();
//        do{
//            *(bool*) VariableTable::GetVariableData("*continue") = false;
//            if(children.get(1))
//                children.get(1)->Exec();
//        }while(CheckConditio((TBranch*)children.get(0)) &&
//               VariableIsFalse("*break") &&
//               VariableIsFalse("*return"));
//        VariableTable::PopVisibilityArea();
//    }
//    if(strcmp(keyword, "break") == 0){
//        *(bool*) VariableTable::GetVariableData("*break") = true;
//    }
//    if(strcmp(keyword, "continue") == 0){
//        *(bool*) VariableTable::GetVariableData("*continue") = true;
//    }
//    if(strcmp(keyword, "return") == 0){
//        *(bool*) VariableTable::GetVariableData("*return") = true;
//        if(children.get(0)){
//            ParametersManager::global_parameters->push(children.get(0)->Exec());
//        }
//    }
//}
