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
            return list;
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
    TSimpleLinkedList<TNode> *global = new TSimpleLinkedList<TNode>;
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

        // get type
        TSimpleLinkedList<LexemeWord> parameters;
        ResultType *type = GetDeclaringType(&parameters);
        if (ErrorReported())
            return nullptr;
        // get name
        LexemeWord *identifier = text->getTyped(curPos++);
        if (!IsValidVarName(identifier)) // not a varname
        {
            ReportError(identifier, "Identifier expected");
            return nullptr;
        }

        // if not function definition
        if (type->baseType->typeOfType != PrimCustFunc::Function ||
                type->p_count > 0 || text->getTyped(curPos)->code != 200) // {
        {
            curPos = pos;
            global->add(HandleDeclaration());
            if (ErrorReported())
                return nullptr;
        }
        else // if function
        {
            TFunctionDefinition *definition = new TFunctionDefinition(identifier);
            definition->signature = static_cast<Func*>(type->baseType);
            definition->implementation = ParseList();
            if (ErrorReported())
                return nullptr;
            for (int i = 0; i < parameters.count(); i++)
            {
                LexemeWord *name = parameters.get(i);
                TFunctionParamsGetter *getter = new TFunctionParamsGetter(name);
                getter->type = static_cast<Func*>(type->baseType)->parameters.get(i);
                definition->implementation->children.insertBefore(getter, i);
            }
            global->add(definition);
        }
    }
    return global;
}
