//
// Created by Alexander on 10-Jun-17.
//

#include "SourceCodeParser.h"
#include "../Types/TypesManager.h"

bool SourceCodeParser::IsValidVarName(LexemeWord *var)
{ return 400 <= var->code && var->code < 600 && !TypesManager::IsType(var->lexeme); }

bool SourceCodeParser::ThrowIfEndOfFile()
{
    if (!IsEnd() && !ErrorReported())
        return false;
    if (!ErrorReported())
        ReportError(text->getTyped(curPos - 1), "Unexpected end of file");
    return true;
}

bool SourceCodeParser::IsMeetType()
{
    int temp = curPos;
    ResultType *result = TryGetResultType();
    curPos = temp;
    return result != nullptr;
}

bool SourceCodeParser::GetParametersInBrackets(TBranch *parent)
{
    // in: curpos is after '('
    // out: curpos is after ')'
    if (text->getTyped(curPos)->code == 205) // )
        return true;
    
    while (true)
    {
        TNode *arg = HandleExpression(true);
        if (arg == nullptr)
        {
            if (!ErrorReported())
                ReportError(text->getTyped(curPos), "Function argument expected");
            return nullptr;
        }
        arg->Parent = parent;
        parent->Children.add(arg);
        if (text->getTyped(curPos)->code != 242) // ,
            break;
        curPos++;
    }
    
    if (text->getTyped(curPos++)->code == 205) // )
        return true;
    
    ReportError(text->getTyped(curPos - 1), "Expected ')' after arguments");
    return false;
}

TNode* SourceCodeParser::GetArrayBrackets()
{
    // in: curpos is after '['
    // out: curpos is after ']'
    TNode *result = HandleExpression(false);
    if (result == nullptr)
    {
        if (!ErrorReported())
            ReportError(text->getTyped(curPos - 1), "Expected array length");
        return nullptr;
    }
    if (text->getTyped(curPos++)->code != 207) // ]
    {
        ReportError(text->getTyped(curPos - 1), "Expected ']'");
        return nullptr;
    }
    return result;
}


ResultType* SourceCodeParser::TryGetResultType()
{
    LexemeWord *typeWord = text->getTyped(curPos);
    if (typeWord->code < 400 || typeWord->code >= 600 || !TypesManager::IsType(typeWord->lexeme))
        return nullptr;
    curPos++;
    
    if (strcmp(typeWord->lexeme, "function") != 0)
    {
        int temp = curPos - 1;
        string name = string(typeWord->lexeme);
        while (text->getTyped(curPos)->code == 208) // .
        {
            curPos++;
            LexemeWord *nextType = text->getTyped(curPos++);
            if (nextType->code < 400 || nextType->code >= 600) // not a varname
            {
                curPos = temp;
                return nullptr;
            }
            name += "." + string(nextType->lexeme);
        }
        
        if (!TypesManager::IsType(name.c_str()))
        {
            curPos = temp;
            return nullptr;
        }
        
        ResultType *result = TypesManager::GetResultType(name.c_str());
        
        if (text->getTyped(curPos)->code != 218) // *
            return result;
        
        result = result->clone();
        while (text->getTyped(curPos)->code == 218) // *
        {
            curPos++;
            result->p_count++;
        }
        return result;
    }
    
    // function
    ResultType *result = new ResultType;
    Func *function = new Func;
    result->baseType = function;
    
    // get function return value
    bool bracketsError = true;
    LexemeWord *brackets = text->getTyped(curPos++);
    if (brackets->code == 204) // (
    {
        function->returnValue = TryGetResultType();
        if (function->returnValue == nullptr)
        {
            if (ErrorReported())
                return nullptr;
            if (text->getTyped(curPos)->code == 205) // )
                function->returnValue = TypesManager::Void();
            else
            {
                ReportError(text->getTyped(curPos), "Expected function return value");
                return nullptr;
            }
        }
        brackets = text->getTyped(curPos++);
        bracketsError = brackets->code != 205; // )
    }
    if (bracketsError)
    {
        ReportError(brackets, "Expected function return type surrounded by ()");
        return nullptr;
    }
    
    // get function params
    if (text->getTyped(curPos++)->code != 204) // (
    {
        ReportError(text->getTyped(curPos - 1), "function parameters must be surrounded by ()");
        return nullptr;
    }
    
    if (text->getTyped(curPos)->code != 205) // )
    {
        while (true)
        {
            ResultType *paramType = TryGetResultType();
            if (paramType == nullptr)
            {
                if (!ErrorReported())
                    ReportError(text->getTyped(curPos), "Expected parameter type");
                return nullptr;
            }
            if (*paramType == *TypesManager::Void())
            {
                ReportError(text->getTyped(curPos - 1),
                            "Declaring parameters of type void is not allowed");
                return nullptr;
            }
            function->parameters.add(paramType);
            
            LexemeWord *next_word = text->getTyped(curPos++);
            if (next_word->code == 205) // )
                break;
            if (next_word->code != 242) // ,
            {
                ReportError(next_word, "Expected ',' between function parameters types");
                return nullptr;
            }
        }
    }
    else
        curPos++;
    return result;
}
