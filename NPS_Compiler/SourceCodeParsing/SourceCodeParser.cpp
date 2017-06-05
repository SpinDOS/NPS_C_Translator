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
    // in: curpos is on {
    // out: curpos is after }
    TList *list = new TList(text->getTyped(curPos++));
    while (true)
    {
        if (ThrowIfEndOfFile())
            return nullptr;
        LexemeWord *word = text->getTyped(curPos);
        
        if (word->code == 201) // }
            break;
        
        if (400 <= word->code && word->code < 600
            && IsMeetType() && !GetDeclaration(&list->children))
            return nullptr; // type declaration
        if (ErrorReported())
            return nullptr;
        
        TNode *sentence = ParseNextSentence();
        if (ErrorReported())
            return nullptr;
        if (sentence == nullptr)
            continue;
        sentence->parent = list;
        list->children.add(sentence);
    }
    curPos++;
    return list;
}


bool SourceCodeParser::GetDeclaration(TSimpleLinkedList<TNode> *list)
{
    // in: curpos is on type
    // out: curpos is after ';'
    ResultType *type = TryGetResultType();
    
    if (ThrowIfEndOfFile())
        return false;
    LexemeWord *name = text->getTyped(curPos++);
    if (name->code == 331) // operator
    {
        if (ThrowIfEndOfFile())
            return false;
        name = text->getTyped(curPos++);
        if ((name->code < 200 || name->code >= 300) && // not a operator symbol
            strcmp(name->lexeme, "implicit") != 0 &&
            strcmp(name->lexeme, "explicit") != 0)
        {
            ReportError(name, "Invalid operator to overload");
            return false;
        }
        if (ThrowIfEndOfFile() || text->getTyped(curPos)->code != 204)
        {
            if (!ErrorReported())
                ReportError(text->getTyped(curPos), "Operator overload must be a function");
            return false;
        }
    }
    else if (!IsValidVarName(name))
    {
        ReportError(name, "Invalid variable name");
        return false;
    }
    else if (ThrowIfEndOfFile())
        return false;

    LexemeWord *next = text->getTyped(curPos++);
    
    // function declaration
    if (next->code == 204) // (
    {
        TFunctionDefinition *functionDefinition = GetFunctionDefinition(type, name);
        if (ErrorReported())
            return false;
        list->add(functionDefinition);
        return true;
    }
    
    ResultType *underlying = type->clone();
    underlying->p_count = 0;
    bool used = false;
    
    while (true)
    {
        if (*type == *TypesManager::Void())
        {
            ReportError(name, "Can not declare variable or array of type void");
            return false;
        }
        
        if (next->code != 206) // [
        {
            TVariableDeclaration *result = new TVariableDeclaration(name);
            result->declaring_type = type;
            list->add(result);
            if (next->code == 241) // =
            {
                curPos -= 2; // revert to name
                list->add(HandleExpression(true));
                if (ErrorReported())
                    return false;
                
                if (ThrowIfEndOfFile())
                    return false;
                next = text->getTyped(curPos++);
            }
        }
        else
        { // array
            TArrayDeclaration *result = new TArrayDeclaration(name);
            result->declaring_type = type;
            result->array_length = HandleExpression(false);
            if (result->array_length == nullptr)
            {
                if (!ErrorReported())
                    ReportError(text->getTyped(curPos - 1), "Expected array length");
                return false;
            }
            if (IsEnd() || text->getTyped(curPos++)->code != 207) // ]
            {
                ReportError(text->getTyped(curPos - 1), "Expected ']'");
                return false;
            }
            list->add(result);
            
            if (ThrowIfEndOfFile())
                return false;
            next = text->getTyped(curPos++);
        }
        
        if (next->code == 243) // ;
            break;
        else if (next->code != 242) // ,
        {
            ReportError(next, "Expected ',' or ';' in the declaration list");
            return false;
        }
        
        if (ThrowIfEndOfFile())
            return false;
        if (text->getTyped(curPos)->code != 218) // *
        {
            type = underlying;
            used = true;
        }
        else
        {
            type = underlying->clone();
            while(text->getTyped(curPos)->code == 218)
            {
                type->p_count++;
                curPos++;
                if (ThrowIfEndOfFile())
                    return false;
            }
        }
        
        if (ThrowIfEndOfFile())
            return false;
        name = text->getTyped(curPos++);
        if (!IsValidVarName(name))
        {
            ReportError(name, "Invalid variable name");
            return false;
        }
        if (ThrowIfEndOfFile())
            return false;
        next = text->getTyped(curPos++);
    }
    if (!used)
        delete underlying;
    return true;
}

TFunctionDefinition* SourceCodeParser::GetFunctionDefinition(ResultType *readBeforeReturnType, LexemeWord *name)
{
    // expected that 'ReturnType Name (' is read before
    TFunctionDefinition *result = new TFunctionDefinition(name);
    result->returnValue = readBeforeReturnType;
    
    if (ThrowIfEndOfFile())
        return nullptr;
    if (text->getTyped(curPos)->code != 205) // )
    {
        while (true)
        {
            FunctionParameterInfo *param = new FunctionParameterInfo;
            result->parameters.add(param);
            
            if (ThrowIfEndOfFile())
                return false;
            param->type = TryGetResultType();
            if (param->type == nullptr)
            {
                if (!ErrorReported())
                    ReportError(text->getTyped(curPos), "Parameter type expected");
                return nullptr;
            }
            if (*param->type == *TypesManager::Void())
            {
                ReportError(text->getTyped(curPos - 1), "Can not declare parameter of type void");
                return nullptr;
            }
            
            if (ThrowIfEndOfFile())
                return nullptr;
            param->name = text->getTyped(curPos++);
            if (!IsValidVarName(param->name))
            {
                ReportError(param->name, "Invalid parameter name");
                return nullptr;
            }
            for (int i = 0; i < result->parameters.count() - 1; i++)
                if (strcmp(result->parameters.get(i)->name, param->name) == 0)
                {
                    ReportError(name, "Paramater with the same name already exists");
                    return nullptr;
                }
            
            if (ThrowIfEndOfFile())
                return nullptr;
            int next_code = text->getTyped(curPos++)->code;
            if (next_code == 241) // =
            {
                param->default_value = HandleExpression(true);
                if (param->default_value == nullptr)
                {
                    if (!ErrorReported())
                        ReportError(text->getTyped(curPos), "Expected parameter default value");
                    return nullptr;
                }
                if (!param->default_value->IsConstantValue())
                {
                    ReportError(text->getTyped(curPos), "Parameter default value must be constant");
                    return nullptr;
                }
                
                if (ThrowIfEndOfFile())
                    return nullptr;
                next_code = text->getTyped(curPos++)->code;
            }
            if (next_code == 205) // )
                break;
            if (next_code != 242) // ,
            {
                ReportError(text->getTyped(curPos - 1), "Expected ',' or ')'");
                return nullptr;
            }
        }
    }
    else
        curPos++;
    
    if (ThrowIfEndOfFile())
        return nullptr;
    if (text->getTyped(curPos)->code != 200) // {
    {
        ReportError(text->getTyped(curPos), "Function implementation expected");
        return nullptr;
    }
    result->implementation = ParseList();
    if (ErrorReported())
        return nullptr;
    return result;
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
                definition->implementation->children.insertBefore(getter, i);
            }
            global->add(definition);
        }
    }
    return global;
}
