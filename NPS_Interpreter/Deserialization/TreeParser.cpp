#include <iostream>
#include "TreeParser.h"
#include "../Operations/OperationManager.h"
#include "../../NPS_library/utils/string_utils.h"

using namespace std;

TSimpleLinkedList<TNode>* TreeParser::Deserialize(char *path) {
    TiXmlDocument doc(path);
    if(!doc.LoadFile())
        return nullptr;
    instructions = new TSimpleLinkedList<TNode>;
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
        parent = (TBranch *) result;
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
    TConstant* result = new TConstant;
    const char *text = element->GetText();
    switch (stoi(element->Attribute("constant_type")))
    {
        case TypeChar:
            result->data = (char *) Heap::get_mem(sizeof(char));
            memcpy(result->data, text, sizeof(char));
            break;
        case TypeString:
            result->data = copy_string(text);
            break;
        case TypeBool:
            {
                bool b = strcmp(text, "true") == 0;
                result->data = (char *) Heap::get_mem(sizeof(bool));
                memcpy(result->data, &b, sizeof(bool));
            }
            break;
        case TypePointer:
            result->data = (char *) Heap::get_mem(sizeof(void*));
            memset(result->data, 0, sizeof(void*));
            break;
        case TypeInt:
            {
                int num = stoi(text);
                result->data = (char *) Heap::get_mem(sizeof(int));
                memcpy(result->data, &num, sizeof(int));
            }
            break;
        case TypeDouble:
            {
                double num = stod(text);
                result->data = (char *) Heap::get_mem(sizeof(double));
                memcpy(result->data, &num, sizeof(double));
            }
            break;
    }
    return result;
}

TVariable* TreeParser::TVariableParser(TiXmlElement *element)
{
    TVariable* result = new TVariable;
    result->name = copy_string(element->GetText());
    return result;
}

TDeclaration* TreeParser::TDeclarationParser(TiXmlElement *element)
{
    TDeclaration* result = new TDeclaration;
    result->name = copy_string(element->Attribute("name"));
    result->size = get_size(element);
    if (!strcmp(element->Attribute("isArray"), "1"))
    {
        TList root;
        Parse(element->FirstChild()->ToElement(), &root);
        result->arrayLength = root.children.takeFirst();
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
    TFunction *result = new TFunction;
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
    return result;
}

TOperation* TreeParser::TKeywordParser(TiXmlElement *element)
{
    InterpreterTNodeType keyword = (InterpreterTNodeType) stoi(element->Attribute("keyword"));
    TOperation* result = OperationManager::GetTOperation(keyword);
    if (keyword == KeywordReturn)
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