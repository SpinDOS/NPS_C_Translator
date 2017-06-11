
#include <iostream>
#include "TNode.h"
#include "../Types/TypesManager.h"
#include "../../NPS_library/InterpreterTNodeType.h"

using namespace std;
//
//void add_size(TiXmlElement *element, NPS_Compiler::TNode *node)
//{
//    int size = TypesManager::GetTypeInfo(node->GetType())->size;
//    element->SetAttribute("size", size);
//}
//
//namespace NPS_Compiler
//{
//    void SerializeNull(TiXmlElement *parent)
//    { parent->LinkEndChild(new TiXmlElement("TEmpty")); }
//
//    void TVariable::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element = new TiXmlElement("TVariable");
//        element->SetAttribute("name", this->Lexeme->lexeme);
//        add_size(element, this);
//        parent->LinkEndChild(element);
//    }
//
//    void TConstant::Serialize(TiXmlElement *parent)
//    {
//        string str;
//        NPS_Interpreter::TConstantType type_of_constant;
//        ResultType str_type;
//        str_type.baseType = TypesManager::Char()->baseType;
//        str_type.p_count = 1;
//
//        if (*constantType == str_type && this->Data != nullptr)
//        {
//            str = string(static_cast<char *>(this->Data));
//            type_of_constant = NPS_Interpreter::TConstantType::TypeString;
//        }
//        else if (constantType->p_count > 0)
//        {
//            str = "null";
//            type_of_constant = NPS_Interpreter::TConstantType::TypePointer;
//        }
//        else if (*constantType == *TypesManager::Bool())
//        {
//            str = string(this->Lexeme->lexeme);
//            type_of_constant = NPS_Interpreter::TConstantType::TypeBool;
//        }
//        else if (*constantType == *TypesManager::Char())
//        {
//            str = string(1, *static_cast<char *>(this->Data));
//            type_of_constant = NPS_Interpreter::TConstantType::TypeChar;
//        }
//        else if (*constantType == *TypesManager::Int())
//        {
//            str = to_string(*static_cast<int *>(this->Data));
//            type_of_constant = NPS_Interpreter::TConstantType::TypeInt;
//        }
//        else
//        {
//            str = to_string(*static_cast<double *>(this->Data));
//            type_of_constant = NPS_Interpreter::TConstantType::TypeDouble;
//        }
//
//        TiXmlElement* element = new TiXmlElement("TConstant");
//        element->SetAttribute("constant_type", type_of_constant);
//        add_size(element, this);
//        str = "_" + str + "_"; // fix tinyxml bug when only escaped chars are used
//        element->LinkEndChild(new TiXmlText(str.c_str()));
//        parent->LinkEndChild(element);
//    }
//
//    void TFunctionCall::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element = new TiXmlElement("TFunctionCall");
//        parent->LinkEndChild(element);
//        add_size(element, this);
//        FunctionToCall->Serialize(element);
//        for(int i = 0; i < Children.count(); i++){
//            Children.get(i)->Serialize(element);
//        }
//    }
//
//    void TOperation::Serialize(TiXmlElement *parent)
//    {
//        int operands_size = 0;
//        if (this->Lexeme->code == 241) // =
//            operands_size = TypesManager::GetTypeInfo
//                    (this->Children.getFirst()->GetType())->size;
//        else
//        {
//            ResultType *pointer_type = nullptr;
//            if (this->Lexeme->code == 202 || this->Lexeme->code == 203 ||  // ++ --
//                 this->Lexeme->code == 221 || this->Lexeme->code == 222) // + -
//                pointer_type = this->Children.getFirst()->GetType();
//
//            if (pointer_type != nullptr && pointer_type->p_count > 0)
//            {
//                pointer_type->p_count--;
//                operands_size = TypesManager::GetTypeInfo(pointer_type)->size;
//                pointer_type->p_count++;
//            }
//        }
//
//        TiXmlElement* element = new TiXmlElement("TOperation");
//        element->SetAttribute("method", this->InterpreterType);
//        add_size(element, this);
//        element->SetAttribute("operands_size", operands_size);
//        parent->LinkEndChild(element);
//        for(int i = 0; i < Children.count(); i++){
//            Children.get(i)->Serialize(element);
//        }
//    }
//
//    void TFunctionDefinition::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element = new TiXmlElement("TFunctionDefinition");
//        element->SetAttribute("name", Lexeme->lexeme);
//        parent->LinkEndChild(element);
//        for(int i = 0; i < Implementation->Children.count(); i++){
//            Implementation->Children.get(i)->Serialize(element);
//        }
//    }
//
//    void TDeclaration::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element;
//        if (this->tNodeType == TNodeTypeParamsGetter)
//            element = new TiXmlElement("TFunctionParamsGetter");
//        else
//        {
//            element = new TiXmlElement("TDeclaration");
//            element->SetAttribute("is_array", arrayLength ? 1 : 0);
//            if (this->arrayLength)
//            {
//                this->type->p_count--;
//                int underlying_size = TypesManager::GetTypeInfo(this->type)->size;
//                this->type->p_count++;
//                this->arrayLength->Serialize(element);
//                element->FirstChild()->ToElement()->SetAttribute("size", underlying_size);
//            }
//        }
//        add_size(element, this);
//        element->SetAttribute("name", this->lexeme->lexeme);
//        parent->LinkEndChild(element);
//    }
//
//    void TList::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element = new TiXmlElement("TList");
//        parent->LinkEndChild(element);
//        for(int i = 0; i < Children.count(); i++)
//            Children.get(i)->Serialize(element);
//    }
//
//    void TKeyword::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element = new TiXmlElement("TKeyword");
//        element->SetAttribute("keyword", this->InterpreterType);
//        int size = 0;
//        if (this->InterpreterType == NPS_Interpreter::InterpreterTNodeType::KeywordReturn &&  // return
//            this->Children.count() > 0)
//                size = TypesManager::GetTypeInfo(this->Children.getFirst()->GetType())->size;
//
//        element->SetAttribute("size", size);
//        parent->LinkEndChild(element);
//        for(int i = 0; i < Children.count(); i++)
//        {
//            TNode *node = this->Children.get(i);
//            if(node)
//                node->Serialize(element);
//            else
//                SerializeNull(element);
//        }
//    }
//
//    void TSwitchCase::Serialize(TiXmlElement *parent)
//    {
//        TiXmlElement* element = new TiXmlElement("TSwitchCase");
//        element->SetAttribute("isDefault", this->isDefault? 1: 0);
//        element->SetAttribute("case", this->caseNum);
//        element->SetAttribute("line", this->lineNum);
//        parent->LinkEndChild(element);
//    }
//}
