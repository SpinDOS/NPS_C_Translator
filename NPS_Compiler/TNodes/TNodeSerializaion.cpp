
#include <iostream>
#include "TNode.h"
#include "../Types/TypesManager.h"
#include "../../NPS_library/InterpreterTNodeType.h"

using namespace std;

void add_size(TiXmlElement *element, NPS_Compiler::TNode *node)
{
    int size = TypesManager::GetTypeInfo(node->getType())->size;
    element->SetAttribute("size", size);
}

namespace NPS_Compiler
{
    void SerializeNull(TiXmlElement *parent)
    { parent->LinkEndChild(new TiXmlElement("TEmpty")); }

    void TVariable::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TVariable");
        element->SetAttribute("name", this->lexeme->lexeme);
        add_size(element, this);
        parent->LinkEndChild(element);
    }

    void TConstant::Serialize(TiXmlElement *parent)
    {
        string str;
        NPS_Interpreter::TConstantType type_of_constant;
        ResultType str_type;
        str_type.baseType = TypesManager::Char()->baseType;
        str_type.p_count = 1;
        
        if (*constantType == str_type && this->data != nullptr)
        {
            str = string(static_cast<char *>(this->data));
            type_of_constant = NPS_Interpreter::TConstantType::TypeString;
        }
        else if (constantType->p_count > 0)
        {
            str = "null";
            type_of_constant = NPS_Interpreter::TConstantType::TypePointer;
        }
        else if (*constantType == *TypesManager::Bool())
        {
            str = string(this->lexeme->lexeme);
            type_of_constant = NPS_Interpreter::TConstantType::TypeBool;
        }
        else if (*constantType == *TypesManager::Char())
        {
            str = string(1, *static_cast<char *>(this->data));
            type_of_constant = NPS_Interpreter::TConstantType::TypeChar;
        }
        else if (*constantType == *TypesManager::Int())
        {
            str = to_string(*static_cast<int *>(this->data));
            type_of_constant = NPS_Interpreter::TConstantType::TypeInt;
        }
        else
        {
            str = to_string(*static_cast<double *>(this->data));
            type_of_constant = NPS_Interpreter::TConstantType::TypeDouble;
        }

        TiXmlElement* element = new TiXmlElement("TConstant");
        element->SetAttribute("constant_type", type_of_constant);
        add_size(element, this);
        str = "_" + str + "_"; // fix tinyxml bug when only escaped chars are used
        element->LinkEndChild(new TiXmlText(str.c_str()));
        parent->LinkEndChild(element);
    }

    void TFunction::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunction");
        parent->LinkEndChild(element);
        add_size(element, this);
        function->Serialize(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TOperation::Serialize(TiXmlElement *parent)
    {
        int operands_size = 0;
        if (this->lexeme->code == 241) // =
            operands_size = TypesManager::GetTypeInfo
                    (this->children.getFirst()->getType())->size;
        else
        {
            ResultType *pointer_type = nullptr;
            if (this->lexeme->code == 202 || this->lexeme->code == 203 ||  // ++ --
                 this->lexeme->code == 221 || this->lexeme->code == 222) // + -
                pointer_type = this->children.getFirst()->getType();

            if (pointer_type != nullptr && pointer_type->p_count > 0)
            {
                pointer_type->p_count--;
                operands_size = TypesManager::GetTypeInfo(pointer_type)->size;
                pointer_type->p_count++;
            }
        }
        
        TiXmlElement* element = new TiXmlElement("TOperation");
        element->SetAttribute("method", this->intepreterTNodeType);
        add_size(element, this);
        element->SetAttribute("operands_size", operands_size);
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TFunctionDefinition::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunctionDefinition");
        element->SetAttribute("name", lexeme->lexeme);
        parent->LinkEndChild(element);
        for(int i = 0; i < implementation->children.count(); i++){
            implementation->children.get(i)->Serialize(element);
        }
    }

    void TDeclaration::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element;
        if (this->tNodeType == TNodeTypeParamsGetter)
            element = new TiXmlElement("TFunctionParamsGetter");
        else
        {
            element = new TiXmlElement("TDeclaration");
            element->SetAttribute("is_array", arrayLength ? 1 : 0);
            if (this->arrayLength)
            {
                this->type->p_count--;
                int underlying_size = TypesManager::GetTypeInfo(this->type)->size;
                this->type->p_count++;
                this->arrayLength->Serialize(element);
                element->FirstChild()->ToElement()->SetAttribute("size", underlying_size);
            }
        }
        add_size(element, this);
        element->SetAttribute("name", this->lexeme->lexeme);
        parent->LinkEndChild(element);
    }

    void TList::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TList");
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++)
            children.get(i)->Serialize(element);
    }

    void TKeyword::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TKeyword");
        element->SetAttribute("keyword", this->intepreterTNodeType);
        int size = 0;
        if (this->intepreterTNodeType == NPS_Interpreter::InterpreterTNodeType::KeywordReturn &&  // return
            this->children.count() > 0)
                size = TypesManager::GetTypeInfo(this->children.getFirst()->getType())->size;

        element->SetAttribute("size", size);
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++)
        {
            TNode *node = this->children.get(i);
            if(node)
                node->Serialize(element);
            else
                SerializeNull(element);
        }
    }
    
    void TSwitchCase::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TSwitchCase");
        element->SetAttribute("isDefault", this->isDefault? 1: 0);
        element->SetAttribute("case", this->caseNum);
        element->SetAttribute("line", this->lineNum);
        parent->LinkEndChild(element);
    }
}
