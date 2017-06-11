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

bool SourceCodeParser::GetDeclaration(TBranch *parent, bool functionsAllowed)
{
    // in: curpos is on type
    // out: curpos is after ';'
    ResultType *type = TryGetResultType();
    
    LexemeWord *name = text->getTyped(curPos++);
    bool mustBeFunction = false;
    if (name->code == 331) // operator
    {
        name = text->getTyped(curPos++);
        if (name->code < 200 || name->code >= 300) // not a operator symbol
        {
            ReportError(name, "Invalid operator to overload");
            return false;
        }
        mustBeFunction = true;
    }
    else if (name->code == 316 || name->code == 319) // implicit explicit
        mustBeFunction = true;
    else if (!IsValidVarName(name))
    {
        ReportError(name, "Invalid variable name");
        return false;
    }

    LexemeWord *next = text->getTyped(curPos++);
    
    // function declaration
    if (next->code == 204 && (text->getTyped(curPos)->code == 205 || IsMeetType())) // (   )
    {
        if (!functionsAllowed)
        {
            ReportError(name, "Function definition is not allowed here");
            return false;
        }
        TFunctionDefinition *functionDefinition = GetFunctionDefinition(type, name);
        if (ErrorReported())
            return false;
        functionDefinition->Parent = parent;
        parent->Children.add(functionDefinition);
        return true;
    }
    if (ErrorReported())
        return false;
    if (mustBeFunction)
    {
        ReportError(name, "This member must be a function");
        return false;
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
    
        TVariableDeclaration *result = new TVariableDeclaration(name);
        result->DeclaringType = type;
        result->Parent = parent;
        parent->Children.add(result);
        
        if (next->code == 206) // [
        {
            result->Array_length = GetArrayBrackets();
            if (ErrorReported())
                return false;
            next = text->getTyped(curPos++);
        }
        
        if (next->code == 204) // (
        {
            if (!GetParametersInBrackets(result))
                return false;
            next = text->getTyped(curPos++);
        }
        else if (next->code == 241) // =
        {
            if (result->Array_length != nullptr)
            {
                ReportError(next, "Assignment is unavailable with array declaration");
                return nullptr;
            }
            TNode *initialization = HandleExpression(true);
            if (ErrorReported())
                return false;
            initialization->Parent = result;
            result->Children.add(initialization);
            next = text->getTyped(curPos++);
        }
        
        if (next->code == 243) // ;
            break;
        else if (next->code != 242) // ,
        {
            ReportError(next, "Expected ',' or ';' in the declaration list");
            return false;
        }
        
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
            }
        }
        
        name = text->getTyped(curPos++);
        if (!IsValidVarName(name))
        {
            ReportError(name, "Invalid variable name");
            return false;
        }
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
    result->ReturnType = readBeforeReturnType;
    
    if (text->getTyped(curPos)->code != 205) // )
    {
        while (true)
        {
            FunctionParameterInfo *param = new FunctionParameterInfo;
            result->Parameters.add(param);
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
            
            param->name = text->getTyped(curPos++);
            if (!IsValidVarName(param->name))
            {
                ReportError(param->name, "Invalid parameter name");
                return nullptr;
            }
            for (int i = 0; i < result->Parameters.count() - 1; i++)
                if (strcmp(result->Parameters.get(i)->name->lexeme, param->name->lexeme) == 0)
                {
                    ReportError(name, "Paramater with the same name already exists");
                    return nullptr;
                }
            
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
    
    if (text->getTyped(curPos)->code != 200) // {
    {
        ReportError(text->getTyped(curPos), "Function implementation expected");
        return nullptr;
    }
    result->Implementation = ParseList();
    if (ErrorReported())
        return nullptr;
    return result;
}


TFictiveRoot* SourceCodeParser::ParseWholeText()
{
    LexemeWord barrier;
    barrier.code = 201; // }
    text->add(&barrier);
    if (!GetAllTypeDeclarations(""))
        return nullptr;
    if (!IsEnd())
    {
        ReportError(text->getTyped(curPos - 1), "Unexpected '}'");
        return nullptr;
    }
    text->take_last();
    curPos = 0;
    
    TFictiveRoot *global = new TFictiveRoot;
    
    while (!IsEnd())
    {
        LexemeWord *lexeme = text->getTyped(curPos);
        if (lexeme->code == 325 || lexeme->code == 304) // struct class
        {
            ReportError(lexeme, "Type declaration is not supported");
            return nullptr;
        }
        else if (!TypesManager::IsType(lexeme->lexeme))
        {
            ReportError(lexeme, "Only type declaration, global variables "
                    "and function definition are allowed in global area");
            return nullptr;
        }
        else if (!GetDeclaration(global, true))
            return nullptr;

//        TNode *last = global->Children.getLast();
//        if (dynamic_cast<TFunctionDefinition*>(last) != nullptr)
//            global->Children.takeLast();
//        else
//            continue;
//
//        // move operator overload to OperationsManager
//        if (!FunctionsManager::AddFunction())
//            return nullptr;
    }
    return global;
}

bool SourceCodeParser::GetAllTypeDeclarations(const char *currentNamespace)
{
    int codeBlocksCount = 0;
    while (true)
    {
        if (ThrowIfEndOfFile())
            return false;
        int meet_lexeme = text->getTyped(curPos++)->code;
        
        if (meet_lexeme == 201 && codeBlocksCount-- == 0) // }
            return true;
        else if (meet_lexeme == 200) // {
            codeBlocksCount++;

        if (meet_lexeme != 325 && meet_lexeme != 304) // struct class
            continue;
        
        LexemeWord *name = text->getTyped(curPos++);
        if (name->code < 400 || name->code >= 600)
        {
            ReportError(text->getTyped(curPos - 1), "Invalid type name");
            return false;
        }
        
        if (text->getTyped(curPos++)->code != 200) // {
        {
            if (!ErrorReported())
                ReportError(text->getTyped(curPos - 1), "Expected class declaration");
            return false;
        }
        
        string fullName = string(currentNamespace) + "." + string(name->lexeme);
        if (!TypesManager::AddTypeInfo(new TypeInfo(fullName.c_str())))
        {
            ReportError(name, "This type is already declared");
            return false;
        }
        if (!GetAllTypeDeclarations(fullName.c_str()))
            return false;
    }
    return true;
}