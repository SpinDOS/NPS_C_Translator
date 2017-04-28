
#include <iostream>
#include "TNode.h"
#include "../../NPS_Interpreter/Functions/ParameterManager.h"

using namespace std;


namespace NPS_Compiler
{

    void TVariable::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TVariable");
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TConstant::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TConstant");
        element->SetAttribute("code", lexeme->code);
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TFunction::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunction");
        element->SetAttribute("lexeme", lexeme->lexeme);
        parent->LinkEndChild(element);
        function->Serialize(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TOperation::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TOperation");
        element->SetAttribute("lexeme", lexeme->lexeme);
        InterpreterTNodeType method = NotDefined;
        if(NumOfChildren == 2)
        {
            switch (lexeme->code)
            {
                case 221:
                    method = BinaryPlusDoubles;
                    break;
                case 222:
                    method = BinaryMinusDoubles;
                    break;
                case 218:
                    method = MultiplyDoubles;
                    break;
                case 219:
                    method = DivideDoubles;
                    break;
                case 241:
                    method = Assignment;
                    size = sizeof(double);
                    break;
                case 225:
                    method = CmpLessDoubles;
                    break;
                case 227:
                    method = CmpMoreDoubles;
                    break;
                case 229:
                    method = CmpEqualDoubles;
                    break;
                case 233:
                    method = CmpNotEqualDoubles;
                    break;
                case 242:
                    method = Comma;
                    break;
            }
        }
        if(NumOfChildren == 1)
        {
            if(lexeme->code == 202){
                if(Priority == 22){
                    method = PostfixIncDouble;
                }
                else{
                    method = PrefixIncDouble;
                }
            }
            if(lexeme->code == 203){
                if(Priority == 22){
                    method = PostfixDecDouble;
                }
                else{
                    method = PrefixDecDouble;
                }
            }
        }
        element->SetAttribute("method_enum", method);
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
        element->SetAttribute("code", lexeme->code);
        parent->LinkEndChild(element);
        for(int i = 0; i < implementation->children.count(); i++){
            implementation->children.get(i)->Serialize(element);
        }
    }

    void TFunctionParamsGetter::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TFunctionParamsGetter");
        element->SetAttribute("size", sizeof(double)); // TODO other size get from typesManager
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TDeclaration::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TDeclaration");
        int size = 0;
        if(this->type->baseType->typeOfType == Function)
            size = sizeof(NPS_Interpreter::Func);
        else
            size = sizeof(double);
        element->SetAttribute("size", size); // TODO other size get from typesManager
        element->LinkEndChild(new TiXmlText(lexeme->lexeme));
        parent->LinkEndChild(element);
    }

    void TList::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TList");
        element->SetAttribute("lexeme", lexeme->lexeme);
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++){
            children.get(i)->Serialize(element);
        }
    }

    void TKeyword::Serialize(TiXmlElement *parent)
    {
        TiXmlElement* element = new TiXmlElement("TKeyWord");
        element->SetAttribute("keyword", lexeme->lexeme);
        parent->LinkEndChild(element);
        for(int i = 0; i < children.count(); i++){
            if(children.get(i))
                children.get(i)->Serialize(element);
        }
    }
}
