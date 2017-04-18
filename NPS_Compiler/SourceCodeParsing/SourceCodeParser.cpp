//
// Created by Alexander on 25-Mar-17.
//

#include <iostream>
#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../Operations/FunctionsManager.h"

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
    // GetAllFunctionDeclarations();
    TSimpleLinkedList<TNode> *initializations = new TSimpleLinkedList<TNode>;
    TSimpleLinkedList<LexemeWord> vars;
    TSimpleLinkedList<LexemeWord> functions;
    while (!IsEnd())
    {
        LexemeWord *lexeme = text->getTyped(curPos);
        if (lexeme->code == 325 || lexeme->code == 304) // struct class
        {
            ReportError(lexeme, "Type declaration is not supported");
            return nullptr;
        }
        else if (TypesManager::GetTypeInfo(lexeme->lexeme) == nullptr)
        {
            ReportError(lexeme, "Only type declaration, global variable initialization "
                    "and function definition are allowed in global area");
            return nullptr;
        }

        int pos = curPos + 1;
        while (text->getTyped(pos)->code == 218) // *
            pos++;
        LexemeWord *identifier = text->getTyped(pos++);
        if (identifier->code < 400 || identifier->code >= 600) // not a varname
        {
            ReportError(identifier, "Identifier expected");
            return nullptr;
        }

        bool identifierUsedForVar = false;
        for (int i = 0; !identifierUsedForVar && i < vars.count(); i++)
            if (strcmp(vars.get(i)->lexeme, identifier->lexeme) == 0)
                identifierUsedForVar = true;
        bool identifierUsedForFunction = false;
        for (int i = 0; !identifierUsedForFunction && i < functions.count(); i++)
            if (strcmp(functions.get(i)->lexeme, identifier->lexeme) == 0)
                identifierUsedForFunction = true;

        bool isFunction = text->getTyped(pos)->code == 204; // (
        if (identifierUsedForVar || (!isFunction && identifierUsedForFunction))
        {
            ReportError(identifier, "Identifier is already in use");
            return nullptr;
        }
        if (isFunction)
        {
            if (!identifierUsedForFunction)
                functions.add(identifier);

        }
        else
        {
            vars.add(identifier);
            initializations->add(HandleDeclaration());
        }
        if (ErrorReported())
            return nullptr;
    }
//    for (int i = 0; i < functionImplementations.count(); i++)
//    {
//        KeyValuePair<int, TNode*> *pair = functionImplementations.get(i);
//        curPos = *pair->key;
//        *pair->value = ParseList();
//        if (ErrorReported())
//            return nullptr;
//    }
    curPos = text->count();
    return initializations;
}
