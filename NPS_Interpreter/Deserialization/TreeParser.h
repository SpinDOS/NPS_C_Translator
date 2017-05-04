#ifndef NPS_INTERPRETER_DESERIALIZATOR_H
#define NPS_INTERPRETER_DESERIALIZATOR_H

#include "../../NPS_library/TinyXmlLibrary/tinyxml.h"
#include "../TNodes/TNode.h"

using namespace NPS_Interpreter;

struct TreeParser {
    TSimpleLinkedList<TNode>* instructions = nullptr;
    TSimpleLinkedList<TNode>* Deserialize(char* path);
private:
    void Parse(TiXmlElement* element, TBranch *parent);
    TConstant* TConstantParser(TiXmlElement *element);
    TVariable* TVariableParser(TiXmlElement *element);
    TDeclaration* TDeclarationParser(TiXmlElement *element);
    TFunctionParamsGetter* TFunctionParamsGetterParser(TiXmlElement *element);
    TList* TListParser(TiXmlElement *element);
    TFunction* TFunctionParser(TiXmlElement *element);
    TFunctionDefinition* TFunctionDefinitionParser(TiXmlElement *element);
    TOperation* TOperationParser(TiXmlElement *element);
    TOperation* TKeywordParser(TiXmlElement *element);
    TSwitchCase *TSwitchCaseParser(TiXmlElement *element);
};


#endif //NPS_INTERPRETER_DESERIALIZATOR_H
