
#include <iostream>
#include "TNode.h"

using namespace std;

namespace NPS_Compiler
{
    void TVariable::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TVariable");
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("code", lexeme->code);
        parent->LinkEndChild(element);
    }

    void TConstant::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TConstant");
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("code", lexeme->code);
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TFunction::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunction");
        element->SetAttribute("code", lexeme->code);
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TBranch::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TBranch");
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("code", lexeme->code);
        element->SetAttribute("lexeme", lexeme->lexeme);
        int method = 0;
        if(lexeme->code == 218){ //*
            method = 1; // double * double
        }
        else if(lexeme->code == 221){ // +
            method = 2; // double + double
        }
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TLeaf::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TLeaf");
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("code", lexeme->code);
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TFunctionDefinition::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunctionDefinition");
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("name", lexeme->lexeme);
        element->SetAttribute("code", lexeme->code);
        parent->LinkEndChild(element);
        for(int i = 0; i < implementation->children.count(); i++){
            implementation->children.get(i)->Serialize(element);
        }
    }

    void TFunctionParamsGetter::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunctionParamsGetter");
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("code", lexeme->code);
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TDeclaration::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TDeclaration");
        element->SetAttribute("tNodeType", tNodeType);
        element->SetAttribute("code", lexeme->code);
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }
}
