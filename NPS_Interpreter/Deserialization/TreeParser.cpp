#include <iostream>
#include "TreeParser.h"
#include "../Operations/OperationManager.h"

using namespace std;

TSimpleLinkedList<TNode>* TreeParser::Deserialize(char *path) {
    TiXmlDocument doc(path);
    if(!doc.LoadFile())
        return nullptr;
    instructions = new TSimpleLinkedList<TNode>();
    Parse(doc.FirstChildElement(), nullptr);
    return instructions;
}

int get_size(TiXmlElement *element)
{
    int size = stoi(element->Attribute("size"));;
    if (size == -1)
        return sizeof(void*);
    return size;
}

void TreeParser::Parse(TiXmlElement *element, TBranch* parent)
{
    TNode* result;
    if(!strcmp(element->Value(), "TConstant")){
        result = TConstantParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        return;
    }
    if(!strcmp(element->Value(), "TVariable")){
        result = TVariableParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        return;
    }
    if(!strcmp(element->Value(), "TDeclaration")){
        result = TDeclarationParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        else
            instructions->add(result);
        return;
    }
    if(!strcmp(element->Value(), "TFunctionParamsGetter")){
        result = TFunctionParamsGetterParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        return;
    }
    if(!strcmp(element->Value(), "TKeyword")){
        result = TKeyWordParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        parent = (TBranch *) result;
    }
    if(!strcmp(element->Value(), "TList")){
        result = TListParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        parent = (TBranch *) result;
    }
    if(!strcmp(element->Value(), "TFunction")){
        result = TFunctionParser(element);
        result->parent = parent;
        if(parent)
            parent->children.add(result);
        parent = (TBranch *) result;
    }
    if(!strcmp(element->Value(), "TOperation")) {
        result = TOperationParser(element);
        result->parent = parent;
        if (parent)
            parent->children.add(result);
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
    if (!strcmp(element->Value(), "TEmpty")){
        if (parent)
            parent->children.add(nullptr);
        return;
    }
    for(TiXmlNode* pChild = element->FirstChild(); pChild != 0; pChild = pChild->NextSibling()){
        Parse(pChild->ToElement(), parent);
    }
}


TConstant* TreeParser::TConstantParser(TiXmlElement *element)
{
    TConstant* result = new TConstant();
    TConstantType type_of_constant = (TConstantType) stoi(element->Attribute("constant_type"));
    if(type_of_constant == TConstantType::TypeChar) // char
    {
        result->data = (char *) Heap::get_mem(sizeof(char));
        memcpy(result->data, element->GetText(), sizeof(char));
        return result;
    }
    if(type_of_constant == TConstantType::TypeString) // string
    {
        result->data = copy_string(element->GetText());
        return result;
    }
    if(type_of_constant == TConstantType::TypeBool) // true or false
    {
        bool b = strcmp(element->GetText(), "true") == 0;
        result->data = (char *) Heap::get_mem(sizeof(bool));
        memcpy(result->data, &b, sizeof(bool));
        return result;
    }
    if(type_of_constant == TConstantType::TypePointer) // 0 as pointer
    {
        result->data = (char *) Heap::get_mem(sizeof(void*));
        memset(result->data, 0, sizeof(void*));
        return result;
    }
    if(type_of_constant == TConstantType::TypeInt) // int
    {
        int num = stoi(element->GetText());
        result->data = (char *) Heap::get_mem(sizeof(int));
        memcpy(result->data, &num, sizeof(int));
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
    InterpreterTNodeType method = (InterpreterTNodeType) stoi(element->Attribute("method"));
    result->handler = OperationManager::GetOperation(method);
    result->size = get_size(element);
    return result;
}

TDeclaration* TreeParser::TDeclarationParser(TiXmlElement *element)
{
    TDeclaration* result = new TDeclaration;
    result->key = copy_string(element->Attribute("name"));
    result->size = get_size(element);
    if (!strcmp(element->Attribute("isArray"), "1"))
    {
        TList root;
        Parse(element->FirstChild()->ToElement(), &root);
        result->arrayLength = root.children.takeFirst();
        result->arrayLength->parent = nullptr;
    }
    return result;
}

TVariable* TreeParser::TVariableParser(TiXmlElement *element)
{
    TVariable* result = new TVariable;
    result->key = copy_string(element->GetText());
    return result;
}

TFunction* TreeParser::TFunctionParser(TiXmlElement *element)
{ return new TFunction; }

TFunctionParamsGetter* TreeParser::TFunctionParamsGetterParser(TiXmlElement *element)
{
    TFunctionParamsGetter* result = new TFunctionParamsGetter;
    result->key = copy_string(element->Attribute("name"));
    result->size = stoi(element->Attribute("size"));
    return result;
}

TKeyword* TreeParser::TKeyWordParser(TiXmlElement *element)
{
    TKeyword* result = new TKeyword;
    result->keyword = copy_string(element->Attribute("keyword"));
    return result;
}

TFunctionDefinition* TreeParser::TFunctionDefinitionParser(TiXmlElement *element) {
    TFunctionDefinition* result = new TFunctionDefinition;
    result->name = copy_string(element->Attribute("name"));
    return result;
}

TList* TreeParser::TListParser(TiXmlElement *element){
    return new TList;
}