//
// Created by Alexander on 25-Mar-17.
//

#include <iostream>
#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

SourceCodeParser::SourceCodeParser(TypeList<LexemeWord> *words)
{
    text = words;
    if (words->count() <= 0)
    {
        ReportError(0l,"File does not contain lexemes");
        return;
    }
    LexemeWord *lastWord = words->getTyped(words->count() - 1);
    if (lastWord->code != 243 && lastWord->code != 201) // ; }
        ReportError(lastWord->positionInTheText + strlen(lastWord->lexeme),
                    "Unexpected end of file (missing ';' or '}')");
}

TList* SourceCodeParser::ParseList()
{
    TList *list = new TList(text->getTyped(curPos++));
    while (!IsEnd())
    {
        LexemeWord *word = text->getTyped(curPos);
        if (word->code == 201) // }
        {
            curPos++;
            if (list->children.count() != 0)
                return list;
            delete list;
            return nullptr;
        }
        
        // handle simple sentence
        TNode *sentence = ParseNextSentence(true);
        if (ErrorReported())
            return nullptr;
        
        if (sentence == nullptr)
            continue;
        sentence->parent = list;
        list->children.add(sentence);
    }
    // end of file without }
    LexemeWord *lastLexeme = text->getTyped(text->count() - 1);
    ReportError(lastLexeme->positionInTheText + strlen(lastLexeme->lexeme),
                "Unexpected end of file (missing '}')");
    return nullptr;
}

TSimpleLinkedList<TNode>* SourceCodeParser::ParseWholeText()
{
    // GetAllTypeDeclarations();
    TSimpleLinkedList<TNode> *initializations = new TSimpleLinkedList<TNode>;
    TSimpleLinkedList<LexemeWord> vars;
    TSimpleLinkedList<LexemeWord> functions;
    //TSimpleLinkedList<KeyValuePair<FunctionDefinition, int>> implementations;
    while (!IsEnd())
    {
        int pos = curPos;
        LexemeWord *lexeme = text->getTyped(curPos);
        if (lexeme->code == 325 || lexeme->code == 304) // struct class
        {
            ReportError(lexeme, "Type declaration is not supported");
            return nullptr;
        }
        else if (!TypesManager::IsType(lexeme->lexeme))
        {
            ReportError(lexeme, "Only type declaration, global variable initialization "
                    "and function definition are allowed in global area");
            return nullptr;
        }
        TSimpleLinkedList<LexemeWord> parameters;
        ResultType *type = GetDeclaringType(&parameters);
        LexemeWord *identifier = text->getTyped(curPos++);
        if (identifier->code < 400 || identifier->code >= 600) // not a varname
        {
            ReportError(identifier, "Identifier expected");
            return nullptr;
        }
        if (TypesManager::IsType(identifier->lexeme) /* or used identifier*/)
        {
            ReportError(identifier, "Identifier is already used");
            return nullptr;
        }

        // if not function definition
        if (type->baseType->typeOfType != PrimCustFunc::Function ||
                text->getTyped(curPos)->code != 200)
        {
            curPos = pos;
            initializations->add(HandleDeclaration());
            if (ErrorReported())
                return nullptr;
            continue;
        }

        TDeclaration *declaration = new TDeclaration(identifier);
        declaration->type = type;
        TList *implementation = ParseList();
        if (ErrorReported())
            return nullptr;
        for (int i = 0; i < parameters.count(); i++)
        {
            LexemeWord *name = parameters.get(i);
            TFunctionParamsGetter *getter = new TFunctionParamsGetter(name);
            getter->type = static_cast<Func*>(type->baseType)->parameters.get(i);
            implementation->children.insertBefore(getter, i);
        }

        initializations->add(declaration);
        initializations->add(implementation);
    }
    curPos = text->count();
    return initializations;
}
