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
    GetAllTypeDeclarations();
    if (ErrorReported())
        return nullptr;
    TSimpleLinkedList<TNode> *global = new TSimpleLinkedList<TNode>;
    while (!IsEnd())
    {
        int pos = curPos;
        LexemeWord *lexeme = text->getTyped(curPos);
        if (lexeme->code == 325 || lexeme->code == 304) // struct class
        {
            ParseStructFields();
            if (ErrorReported())
                return nullptr;
            continue;
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
        if (strcmp(identifier->lexeme, "operator") != 0)
        {
            if (!IsValidVarName(identifier)) // not a varname
            {
                ReportError(identifier, "Identifier expected");
                return nullptr;
            }
        }
        else
        {
            identifier = text->getTyped(curPos++);
            if (200 < identifier->code || identifier->code >= 300)
            {
                ReportError(identifier, "Operator expected");
                return nullptr;
            }
            LexemeWord *next = text->getTyped(curPos);
            if ((identifier->code == 204 && next->code == 205) || // ()
                (identifier->code == 206 && next->code == 207)) // []
            {
                string newText = string(identifier->lexeme) + string(next->lexeme);
                Heap::free_mem(identifier->lexeme);
                identifier->lexeme = copy_string(newText.c_str());
                curPos++;
            }
            
            if (type->baseType->typeOfType != PrimCustFunc::Function ||
             type->p_count > 0 || text->getTyped(curPos)->code != 200)
            {
                ReportError(identifier, "Operator implementation expected");
                return nullptr;
            }
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
                definition->implementation->children.insertBefore(getter, 0);
            }
            global->add(definition);
        }
    }
    return global;
}

void SourceCodeParser::GetAllTypeDeclarations()
{
    curPos = 0;
    int count = text->count();
    while (curPos < count) // reserved for 'struct name {}'
    {
        LexemeWord *lexeme = text->getTyped(curPos++);
        if (lexeme->code != 325 && lexeme->code != 304) // struct class
            continue;
        bool is_struct = lexeme->code == 325;
        if (!is_struct) // class
        {
            ReportError(lexeme, "Classes are not supported yet");
            return;
        }
        if (curPos == count)
        {
            ReportError(lexeme, "Type name expected");
            return;
        }
        lexeme = text->getTyped(curPos++);
        if (!IsValidVarName(lexeme))
        {
            ReportError(lexeme, "Invalid type name");
            return;
        }
        if (TypesManager::GetTypeInfo(lexeme->lexeme) != nullptr)
        {
            ReportError(lexeme, "This type is already declared");
            return;
        }
        
        TypeInfo *typeInfo = new TypeInfo(lexeme->lexeme);
        TypesManager::AddTypeInfo(typeInfo);
        
        if (curPos == count || text->getTyped(curPos++)->code != 200) // {
        {
            ReportError(text->getTyped(curPos - 1), "'{' expected");
            return;
        }
        bool meet_end = false;
        while (!meet_end && curPos < count)
        {
            if (text->getTyped(curPos++)->code == 201) // }
                meet_end = true;
        }
        if (!meet_end)
        {
            ReportError(text->getTyped(curPos - 1), "Type declaration is not finished (missing '}')");
            return;
        }
    }
    curPos = 0;
}

void SourceCodeParser::ParseStructFields()
{
    ResultType *this_type = TypesManager::GetResultType(text->getTyped(curPos + 1)->lexeme);
    TypeInfo *type = TypesManager::GetTypeInfo(this_type);
    curPos += 3; // struct name {
    int offset = 0;
    while (true)
    {
        if (text->getTyped(curPos)->code == 201) // }
        {
            curPos++;
            break;
        }
        
        FieldInfo *fieldInfo = reinterpret_cast<FieldInfo*>(Heap::get_mem(sizeof(FieldInfo)));
        fieldInfo->type = GetDeclaringType();
        if (ErrorReported())
            return;
        fieldInfo->offset = offset;
        offset += TypesManager::GetTypeInfo(fieldInfo->type)->size;
        
        LexemeWord *lexeme = text->getTyped(curPos++);
        if (!IsValidVarName(lexeme))
        {
            ReportError(lexeme, "Invalid field name");
            return;
        }
        if (type->fields.get(lexeme->lexeme) != nullptr)
        {
            ReportError(lexeme, "Field name is already used");
            return;
        }
        fieldInfo->name = copy_string(lexeme->lexeme);
        if (text->getTyped(curPos++)->code != 243) // ;
        {
            ReportError(text->getTyped(curPos - 1), "Expected ';' after struct field");
            return;
        }
        type->fields.put(fieldInfo->name, fieldInfo);
        
    }
    type->size = offset;
}

