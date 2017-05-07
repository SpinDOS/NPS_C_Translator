#include <iostream>
#include "TreeParser.h"
#include "../Operations/OperationManager.h"
#include "../../NPS_library/utils/string_utils.h"

using namespace std;

TConstant* get_TNullPointer();
TConstant* get_TBoolConstant(bool b);

TSimpleLinkedList<TNode>* TreeParser::Deserialize(char *path) {
    TiXmlDocument doc(path);
    if(!doc.LoadFile())
        return nullptr;
    instructions = new TSimpleLinkedList<TNode>;
    Parse(doc.FirstChildElement(), nullptr);
    return instructions;
}

int convert_size(int size)
{ return size == -1? sizeof(void*) : size;}

int get_size(TiXmlElement *element)
{ return convert_size(stoi(element->Attribute("size"))); }

void TreeParser::Parse(TiXmlElement *element, TBranch* parent)
{
    TNode* result;
    if (!strcmp(element->Value(), "TEmpty")){
        if (parent)
            parent->children.add(nullptr);
        return;
    }
    else if(!strcmp(element->Value(), "TConstant")){
        result = TConstantParser(element);
        if(parent)
            parent->children.add(result);
        return;
    }
    else if(!strcmp(element->Value(), "TVariable")){
        result = TVariableParser(element);
        if(parent)
            parent->children.add(result);
        return;
    }
    else if(!strcmp(element->Value(), "TDeclaration")){
        result = TDeclarationParser(element);
        if(parent)
            parent->children.add(result);
        else
            instructions->add(result);
        return;
    }
    else if(!strcmp(element->Value(), "TFunctionParamsGetter")){
        result = TFunctionParamsGetterParser(element);
        if(parent)
            parent->children.add(result);
        return;
    }
    else if(!strcmp(element->Value(), "TList")){
        result = TListParser(element);
        if(parent)
            parent->children.add(result);
        parent = (TBranch *) result;
    }
    else if(!strcmp(element->Value(), "TFunction")){
        result = TFunctionParser(element);
        if(parent)
            parent->children.add(result);
        parent = (TBranch*) result;
    }
    else if(!strcmp(element->Value(), "TFunctionDefinition")){
        result = TFunctionDefinitionParser(element);
        instructions->add(result);
        parent = (TBranch *) result;
    }
    else if(!strcmp(element->Value(), "TOperation")) {
        result = TOperationParser(element);
        if (parent)
            parent->children.add(result);
        else
            instructions->add(result);
        parent = (TBranch *) result;
    }
    else if(!strcmp(element->Value(), "TKeyword")){
        result = TKeywordParser(element);
        if(parent)
            parent->children.add(result);
        parent = (TBranch *) result;
    }
    else if(!strcmp(element->Value(), "TSwitchCase")){
        result = TSwitchCaseParser(element);
        parent->children.add(result);
        return;
    }

    for(TiXmlNode* pChild = element->FirstChild(); pChild != 0; pChild = pChild->NextSibling()){
        Parse(pChild->ToElement(), parent);
    }
}


TConstant* TreeParser::TConstantParser(TiXmlElement *element)
{
    TConstant* result;
    string text = string(element->GetText() + 1);
    text = text.substr(0, text.length() - 1); // skip '_'s
    switch (stoi(element->Attribute("constant_type")))
    {
        case TypeChar:
            result = new TConstant;
            result->data = Heap::get_mem(sizeof(char));
            memcpy(result->data, text.c_str(), sizeof(char));
            break;
        case TypeString:
            result = new TConstant;
            result->data = Heap::get_mem(sizeof(char*));
            *reinterpret_cast<char**>(result->data) = copy_string(text.c_str());
            break;
        case TypeBool:
            result = get_TBoolConstant(text == "true");
            break;
        case TypePointer:
            result = get_TNullPointer();
            break;
        case TypeInt:
            {
                int num = stoi(text);
                result = new TConstant;
                result->data = Heap::get_mem(sizeof(int));
                memcpy(result->data, &num, sizeof(int));
            }
            break;
        case TypeDouble:
            {
                double num = stod(text);
                result = new TConstant;
                result->data = Heap::get_mem(sizeof(double));
                memcpy(result->data, &num, sizeof(double));
            }
            break;
        default:
            return nullptr;
    }
    result->size = get_size(element);
    return result;
}

TVariable* TreeParser::TVariableParser(TiXmlElement *element)
{
    TVariable* result = new TVariable;
    result->name = copy_string(element->Attribute("name"));
    result->size = get_size(element);
    return result;
}

TDeclaration* TreeParser::TDeclarationParser(TiXmlElement *element)
{
    TDeclaration* result = new TDeclaration;
    result->name = copy_string(element->Attribute("name"));
    result->size = get_size(element);
    if (!strcmp(element->Attribute("is_array"), "1"))
    {
        TList root;
        Parse(element->FirstChild()->ToElement(), &root);
        result->arrayLength = root.children.takeFirst();
        result->underlying_size = result->arrayLength->size;
        result->arrayLength->size = sizeof(int);
    }
    return result;
}

TFunctionParamsGetter* TreeParser::TFunctionParamsGetterParser(TiXmlElement *element)
{
    TFunctionParamsGetter* result = new TFunctionParamsGetter;
    result->name = copy_string(element->Attribute("name"));
    result->size = get_size(element);
    return result;
}

TList* TreeParser::TListParser(TiXmlElement *element){
    TList *result = new TList;
    return result;
}

TFunction* TreeParser::TFunctionParser(TiXmlElement *element)
{
    int size = get_size(element);
    TFunction *result = new TFunction(size != 0);
    result->size = size;
    return result;
}

TFunctionDefinition* TreeParser::TFunctionDefinitionParser(TiXmlElement *element) {
    TFunctionDefinition* result = new TFunctionDefinition;
    result->name = copy_string(element->Attribute("name"));
    return result;
}

TOperation* TreeParser::TOperationParser(TiXmlElement *element)
{
    InterpreterTNodeType method = (InterpreterTNodeType) stoi(element->Attribute("method"));
    TOperation* result = OperationManager::GetTOperation(method);
    result->size = get_size(element);
    result->operands_size = convert_size(stoi(element->Attribute("operands_size")));
    return result;
}

TBranch* TreeParser::TKeywordParser(TiXmlElement *element)
{
    InterpreterTNodeType keyword = (InterpreterTNodeType) stoi(element->Attribute("keyword"));
    TBranch* result = OperationManager::GetTKeyword(keyword);
    result->size = get_size(element);
    return result;
}

TSwitchCase* TreeParser::TSwitchCaseParser(TiXmlElement *element)
{
    TSwitchCase *result = new TSwitchCase;
    result->is_default = !strcmp(element->Attribute("isDefault"), "1");
    result->case_num = stoi(element->Attribute("case"));
    result->line_num = stoi(element->Attribute("line"));
    return result;
}

struct TNullPointer : TConstant
{
    TNullPointer()
    {
        this->data = Heap::get_mem(sizeof(void*));
        *reinterpret_cast<char**>(this->data) = nullptr;
        this->size = sizeof(void*);
    }
} _tNullPointer;
TConstant* get_TNullPointer(){return &_tNullPointer;}

struct TBoolConstant : TConstant
{
    TBoolConstant(bool b)
    {
        this->size = sizeof(bool);
        this->data = Heap::get_mem(sizeof(bool));
        *reinterpret_cast<bool*>(this->data) = b;
    }
};
TBoolConstant _tBoolTrue(true), _tBoolFalse(false);
TConstant* get_TBoolConstant(bool b) {return b? &_tBoolTrue : &_tBoolFalse;}
