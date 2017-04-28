#include <iostream>
#include "TreeParser.h"

using namespace std;

TSimpleLinkedList<TNode>* TreeParser::Deserialize(char *path) {
    TiXmlDocument doc(path);
    if(!doc.LoadFile()) return 0;
    instructions = new TSimpleLinkedList<TNode>();
    Parse(doc.FirstChildElement(), nullptr);
    return instructions;
}

void TreeParser::Parse(TiXmlElement *element, TBranch* parent)
{
    TNode* result;
    if(!strcmp(element->Value(), "TConstant")){
        result = TConstantParser(element);
        result->parent = parent;
        if(parent)
            parent->children->add(result);
        return;
    }
    if(!strcmp(element->Value(), "TVariable")){
        result = TVariableParser(element);
        result->parent = parent;
        if(parent)
            parent->children->add(result);
        return;
    }
    if(!strcmp(element->Value(), "TDeclaration")){
        result = TDeclarationParser(element);
        result->parent = parent;
        if(parent)
            parent->children->add(result);
        else
            instructions->add(result);
        return;
    }
    if(!strcmp(element->Value(), "TFunctionParamsGetter")){
        result = TFunctionParamsGetterParser(element);
        result->parent = parent;
        if(parent)
            parent->children->add(result);
        return;
    }
    if(!strcmp(element->Value(), "TKeyWord")){
        result = TKeyWordParser(element);
        result->parent = parent;
        if(parent)
            parent->children->add(result);
        parent = (TBranch *) result;
    }
    /*if(!strcmp(element->Value(), "TList")){
        result = TListParser(element);
        if(parent)
            parent->children->add(result);
        parent = (TBranch *) result;
    }*/
    if(!strcmp(element->Value(), "TFunction")){
        result = TFunctionParser(element);
        result->parent = parent;
        if(parent)
            parent->children->add(result);
        parent = (TBranch *) result;
    }
    if(!strcmp(element->Value(), "TOperation")) {
        result = TOperationParser(element);
        result->parent = parent;
        if (parent)
            parent->children->add(result);
        else
            instructions->add(result);
        parent = (TBranch *) result;
    }
    if(!strcmp(element->Value(), "TFunctionDefinition")){
        result = TFunctionDefinitionParser(element);
        result->parent = parent;
        instructions->add(result);
        parent = (TBranch *) result;
    }
    for(TiXmlNode* pChild = element->FirstChild(); pChild != 0; pChild = pChild->NextSibling()){
        Parse(pChild->ToElement(), parent);
    }
}


TConstant* TreeParser::TConstantParser(TiXmlElement *element)
{
    TConstant* result = new TConstant();
    int code = stoi((char *) element->Attribute("code"));
    if(code == 110) // char
    {
        result->data = (char *) Heap::get_mem(1);
        memcpy(result->data, element->GetText(), sizeof(char));
        return result;
    }
    if(code == 100) // string
    {
        result->data = copy_string(element->GetText());
        return result;
    }
    if(code == 150 || code == 151) // true or false
    {
        bool b = code == 150 ? 1 : 0;
        result->data = (char *) Heap::get_mem(1);
        memcpy(result->data, &b, sizeof(bool));
        return result;
    }
    // double (num)
    double num = stod(element->GetText());
    result->data = (char *) Heap::get_mem(sizeof(double));
    memcpy(result->data, &num, sizeof(double));
    return result;
}

TOperation* TreeParser::TOperationParser(TiXmlElement *element)
{
    TOperation* result = new TOperation;
    result->children = new TSimpleLinkedList<TNode>();
    result->method = (InterpreterTNodeType) stoi(element->Attribute("method_enum"));
    result->size = stoi(element->Attribute("size"));
    return result;
}

TDeclaration* TreeParser::TDeclarationParser(TiXmlElement *element)
{
    TDeclaration* result = new TDeclaration;
    result->key = copy_string(element->GetText());
    result->size = stoi(element->Attribute("size"));
    return result;
}

TVariable* TreeParser::TVariableParser(TiXmlElement *element)
{
    TVariable* result = new TVariable;
    result->key = copy_string(element->GetText());
    return result;
}

TFunction* TreeParser::TFunctionParser(TiXmlElement *element)
{
    TFunction* result = new TFunction;
    result->children = new TSimpleLinkedList<TNode>;
    return result;
}

TFunctionParamsGetter* TreeParser::TFunctionParamsGetterParser(TiXmlElement *element)
{
    TFunctionParamsGetter* result = new TFunctionParamsGetter;
    result->key = copy_string(element->GetText());
    result->size = stoi(element->Attribute("size"));
    return result;
}

TKeyword* TreeParser::TKeyWordParser(TiXmlElement *element)
{
    TKeyword* result = new TKeyword;
    result->keyword = copy_string(element->Attribute("keyword"));
    result->children = new TSimpleLinkedList<TNode>;
    return result;
}

TFunctionDefinition* TreeParser::TFunctionDefinitionParser(TiXmlElement *element) {
    TFunctionDefinition* result = new TFunctionDefinition;
    result->children = new TSimpleLinkedList<TNode>;
    result->name = copy_string(element->Attribute("name"));
    return result;
}

//TList* TreeParser::TListParser(TiXmlElement *element){
//    TList* result = new TList;
//    result->children = new TSimpleLinkedList<TNode>;
//    return result;
//}