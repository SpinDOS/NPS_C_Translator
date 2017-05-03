
#include <iostream>
#include "TNode.h"
#include "../../NPS_Interpreter/Functions/ParameterManager.h"
#include "../Types/TypesManager.h"

using namespace std;


namespace NPS_Compiler
{
    void SerializeNull(TiXmlElement *parent)
    { parent->LinkEndChild(new TiXmlElement("TEmpty")); }

    void TVariable::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TVariable");
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TConstant::Serialize(TiXmlElement *parent)
    {
        string str;
        TConstantType type_of_constant;
        ResultType str_type;
        str_type.baseType = TypesManager::Char()->baseType;
        str_type.p_count = 1;
        
        if (*constantType == str_type)
        {
            str = string(static_cast<char *>(this->data));
            type_of_constant = TConstantType::TypeString;
        }
        else if (constantType->p_count > 0)
        {
            str = "null";
            type_of_constant = TConstantType::TypePointer;
        }
        else if (*constantType == *TypesManager::Bool())
        {
            str = string(this->lexeme->lexeme);
            type_of_constant = TConstantType::TypeBool;
        }
        else if (*constantType == *TypesManager::Char())
        {
            str = string(1, *static_cast<char *>(this->data));
            type_of_constant = TConstantType::TypeChar;
        }
        else if (*constantType == *TypesManager::Int())
        {
            str = to_string(*static_cast<int *>(this->data));
            type_of_constant = TConstantType::TypeInt;
        }
        else
        {
            str = to_string(*static_cast<double *>(this->data));
            type_of_constant = TConstantType::TypeDouble;
        }

        TiXmlElement* element = new TiXmlElement("TConstant");
        element->SetAttribute("constant_type", type_of_constant);
        element->LinkEndChild(new TiXmlText(str.c_str()));
        parent->LinkEndChild(element);
    }

    void TFunction::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunction");
        parent->LinkEndChild(element);
        function->Serialize(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TOperation::Serialize(TiXmlElement *parent)
    {
        int size = 0;
        if (this->lexeme->code == 241) // =
            size = TypesManager::GetTypeInfo(this->children.getFirst()->getType())->size;
        else
        {
            ResultType *pointer_type = nullptr;
            if (this->lexeme->code == 202 || this->lexeme->code == 203) // ++ --
                pointer_type = this->children.getFirst()->getType();
            else if (this->children.count() == 2 &&
                    (this->lexeme->code == 221 || this->lexeme->code == 222)) // + -
            {
                pointer_type = this->children.getFirst()->getType();
                if (pointer_type->p_count == 0)
                    pointer_type = this->children.getLast()->getType();
            }
            if (pointer_type != nullptr && pointer_type->p_count > 0)
            {
                pointer_type->p_count--;
                size = TypesManager::GetTypeInfo(pointer_type)->size;
                pointer_type->p_count++;
            }
        }
        
        TiXmlElement* element = new TiXmlElement("TOperation");
        element->SetAttribute("method", this->intepreterTNodeType);
        element->SetAttribute("size", size);
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
            element = new TiXmlElement("TDeclaration");
        element->SetAttribute("size", TypesManager::GetTypeInfo(this->type)->size);
        element->SetAttribute("isArray", arrayLength? 1 : 0);
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
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
        element->SetAttribute("keyword", lexeme->lexeme);
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
