#ifndef NPS_INTERPRETER_DESERIALIZATOR_H
#define NPS_INTERPRETER_DESERIALIZATOR_H


#include "../TNodes/TNode.h"

using namespace NPS_Interpreter;

struct TreeParser {
    TSimpleLinkedList<TNode>* instructions;
    TSimpleLinkedList<TNode>* Deserialize(char* path);

private:
    void Parse(TiXmlElement* element, TBranch *parent);

    TOperation* TOperationParser(TiXmlElement *element);
    TConstant* TConstantParser(TiXmlElement *element);
    TDeclaration* TDeclarationParser(TiXmlElement *element);
    TVariable* TVariableParser(TiXmlElement *element);
    TFunction* TFunctionParser(TiXmlElement *element);
    TFunctionDefinition* TFunctionDefinitionParser(TiXmlElement *element);
    TFunctionParamsGetter* TFunctionParamsGetterParser(TiXmlElement *element);
    TKeyword* TKeyWordParser(TiXmlElement *element);
//    TList* TListParser(TiXmlElement *element);
};


#endif //NPS_INTERPRETER_DESERIALIZATOR_H
