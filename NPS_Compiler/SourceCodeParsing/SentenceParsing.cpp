//
// Created by Alexander on 18-Apr-17.
//

#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight);

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

TNode* SourceCodeParser::ParseNextSentence()
{
    // in: curpos is on first sentence symbol
    // out: curpos is after ';'
    if (ThrowIfEndOfFile())
        return nullptr;
    LexemeWord *word = text->getTyped(curPos);
    if (300 <= word->code && word->code < 400)// keyword
    {
        switch (word->code)
        {
            case 308: // do
                return HandleKeywordDoWhile();
            case 330: // while
                return HandleKeywordWhile();
            case 312: // for
                return HandleKeywordFor();
            case 313: // if
                return HandleKeywordIf();
            case 326: // switch
                return HandleKeywordSwitch();
            case 301: // break
            case 305: // continue
                return HandleKeywordBreakContinue();
            case 323: // return
                return HandleKeywordReturn();
            case 309: // delete
                return HandleKeywordDelete();
            case 318: // new
            case 300: // null
            case 315: // this
                break;
            default:
                ReportError(word, "This keyword can not be used here");
                return nullptr;
        }
    }
    else if (400 <= word->code && word->code < 600 && IsMeetType()) // type declaration
    {
        ReportError(word, "Type declaration is not allowed here");
        return nullptr;
    }
    else if (ErrorReported()) // catch errors of IsMeetType()
        return nullptr;
    else if (word->code == 200) // {
        return ParseList();
    
    // simple sentence
    TNode *result = HandleExpression(false);
    if (ErrorReported())
        return nullptr;
    
    // validate terminating ;
    if (ThrowIfEndOfFile())
        return nullptr;
    word = text->getTyped(curPos++);
    if (word->code != 243) // ;
    {
        ReportError(word, "Expected ';'");
        return nullptr;
    }
    if (result == nullptr)
        return nullptr;
    
    // check for a;
    if (result->tNodeType == TNodeTypeConstant ||
        result->tNodeType == TNodeTypeVariable)
    {
        ReportError(result->lexeme, "Sentence can not contain only constant or variable");
        return nullptr;
    }
    return result;
}

TNode* SourceCodeParser::HandleExpression(bool stopOnComma)
{
    // in: curpos is on first expression symbol
    // out: curpos is on ';' or smth like it
    if (ThrowIfEndOfFile())
        return nullptr;
    TBranch *root = TFictiveRoot::GetFictiveRoot();
    TBranch *cur = root;
    bool hasLeft = false, expectedRight = false;
    while (!ErrorReported() && cur != nullptr)
    {
        if (ThrowIfEndOfFile())
            return nullptr;
        LexemeWord *word = text->getTyped(curPos++);
        if (100 <= word->code && word->code < 200) // constant
            cur = HandleTLeaf(cur, word, hasLeft, expectedRight);
        else if (300 <= word->code && word->code < 400)// keyword
        {
            switch (word->code)
            {
                case 318: // new
                case 300: // null
                case 315: // this
                    break;
                default:
                    ReportError(word, "Unexpected keyword. Maybe you miss ';'?");
                    return nullptr;
            }
            // dont change cur
        }
        else if (400 <= word->code && word->code < 600)// varname
        {
            if (IsMeetType()) // type declaration
                ReportError(word, "Unexpected type declaration. Maybe you miss ';'?");
            if (ErrorReported())
                return nullptr;
            cur = HandleTLeaf(cur, word, hasLeft, expectedRight); // variable
        }
        else if (hasLeft && word->code == 204) // (
            cur = HandleFunctionCall(cur, word, hasLeft, expectedRight); // function call
        else // operation
            cur = HandleOperation(cur, word, hasLeft, expectedRight, stopOnComma);
    }
    if (ErrorReported() || root->children.count() == 0)
        return nullptr;
    TNode *result = root->children.takeFirst();
    result->parent = nullptr;
    return result;
}

TBranch *SourceCodeParser::HandleOperation(TBranch *cur, LexemeWord *word,
                                           bool &hasLeft, bool &expectedRight, bool stopOnComma)
{
    // in: word is operation, curpos is on next symbol
    // out: curpos is on next symbol after operation
    TOperation *operation;
    if (word->code == 204 && IsMeetType()) // (
        operation = GetTypeCast(word, hasLeft, expectedRight);
    else if (ErrorReported()) // handle possible error of IsMeetType()
        return nullptr;
    else
        operation = GetTOperation(word, hasLeft, expectedRight);
    
    if (ErrorReported())
        return nullptr;
    
    // handle :
    if (word->code == 240) // :
    {
        while (operation->Priority > cur->Priority
               || (operation->Priority == cur->Priority && cur->lexeme->code != 239)) // ?
            cur = cur->parent;
        
        delete operation;
        if (cur->lexeme->code != 239) // ?
        {
            ReportError(word, "Invalid using ':' without '?'");
            return nullptr;
        }
        Heap::free_mem(cur->lexeme->lexeme);
        cur->lexeme->lexeme = copy_string("?:");
        cur->lexeme->code = 240;
        return cur;
    }
    
    while (operation->Priority > cur->Priority ||
           (operation->Priority == cur->Priority && cur->IsLeftAssociated))
        if (cur->lexeme->code == 239) // ?
        {
            ReportError(word, "Invalid using operator inside '?:' block");
            return nullptr;
        }
        else
            cur = cur->parent;
    
    // handle on root
    if (cur->parent == nullptr &&
        ((operation->Priority == MAXPRIORITY && operation->tNodeType != TNodeTypeCast) // ; ] )
         || (stopOnComma && word->code == 242))) // ,
    {
        curPos--;
        delete operation;
        return nullptr;
    }
    
    // simple operation
    if (cur->Priority != operation->Priority ||
        (cur->lexeme->code != 204 && cur->lexeme->code != 206)) // ( [
    {
        if (word->code == 204 || word->code == 206) // ( [
            operation->Priority = MAXPRIORITY;
        
        if (operation->IsLeftAssociated || operation->NumOfChildren > 1)
        {
            operation->children.add(cur->children.takeLast());
            operation->children.getLast()->parent = operation;
        }
        cur->children.add(operation);
        operation->parent = cur;
        return operation;
    }
    
    // handle ()
    if (cur->lexeme->code == 204) // (
    {
        int code = operation->lexeme->code;
        delete operation;
        if (code != 205) // )
        {
            ReportError(word, "Expected ')'");
            return nullptr;
        }
        TBranch *parent = cur->parent;
        parent->children.takeLast();
        parent->children.add(cur->children.takeFirst());
        parent->children.getLast()->parent = parent;
        delete cur;
        return parent;
    }
    
    // handle []
    if (cur->lexeme->code == 206) // [
    {
        int code = operation->lexeme->code;
        delete operation;
        if (code != 207) // ]
        {
            ReportError(word, "Expected ']'");
            return nullptr;
        }
        Heap::free_mem(cur->lexeme->lexeme);
        cur->lexeme->lexeme = copy_string("[]");
        return cur->parent;
    }
    
    return nullptr;
}

TOperation* SourceCodeParser::GetTypeCast(LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    // in: curpos is on type name
    // out: curpos is after ')'
    if (hasLeft)
    {
        ReportError(word, "Unexpected type cast. Operation expected");
        return nullptr;
    }
    if (ThrowIfEndOfFile())
        return nullptr;
    int pos = curPos;
    ResultType *targetType = TryGetResultType();
    if (targetType == nullptr)
    {
        if (!ErrorReported())
            ReportError(text->getTyped(pos), "Expected type to cast");
        return nullptr;
    }
    
    // check for terminal )
    if (ThrowIfEndOfFile())
        return nullptr;
    if (text->getTyped(curPos++)->code != 205) // )
    {
        ReportError(text->getTyped(curPos - 1), "Expected ')' after type to cast");
        return nullptr;
    }
    word->code = 205; // )
    Heap::free_mem(word->lexeme);
    word->lexeme = copy_string("(type)");
    hasLeft = false;
    expectedRight = true;
    return new TTypeCast(targetType, word);
}

TBranch *SourceCodeParser::HandleFunctionCall(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    // in: word is '(', curpos is on next position
    // out: curpos is on next symbol after ')'
    
    TFunction *function = new TFunction(word);
    function->parent = cur;
    function->function = cur->children.takeLast();
    function->function->parent = function;
    cur->children.add(function);
    
    // parse arguments
    if (ThrowIfEndOfFile())
        return nullptr;
    if (text->getTyped(curPos)->code != 205) // )
        while (true)
        {
            TNode *arg = HandleExpression(true);
            if (arg == nullptr)
            {
                if (!ErrorReported())
                    ReportError(text->getTyped(curPos), "Function argument expected");
                return nullptr;
            }
            arg->parent = function;
            function->children.add(arg);
            
            if (ThrowIfEndOfFile())
                return nullptr;
            if (text->getTyped(curPos)->code != 242) // ,
                break;
            curPos++;
        }
    
    if (IsEnd() || text->getTyped(curPos++)->code != 205) // )
    {
        ReportError(text->getTyped(curPos - 1), "Expected ')' after function arguments");
        return nullptr;
    }
    function->lexeme->code = 205;
    Heap::free_mem(function->lexeme->lexeme);
    function->lexeme->lexeme = copy_string("()");
    hasLeft = true;
    expectedRight = false;
    return cur;
}




ResultType* SourceCodeParser::TryGetResultType()
{
    if (IsEnd())
        return nullptr;
    LexemeWord *typeWord = text->getTyped(curPos);
    if (!TypesManager::IsType(typeWord->lexeme))
        return nullptr;
    curPos++;
    if (ThrowIfEndOfFile())
        return nullptr;
    
    if (strcmp(typeWord->lexeme, "function") != 0)
    {
        int temp = curPos - 1;
        string name = string(typeWord->lexeme);
        while (text->getTyped(curPos)->code == 208) // .
        {
            curPos++;
            if (ThrowIfEndOfFile())
                return nullptr;
            LexemeWord *nextType = text->getTyped(curPos++);
            if (ThrowIfEndOfFile())
                return nullptr;
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
            if (ThrowIfEndOfFile())
                return nullptr;
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
            if (ThrowIfEndOfFile())
                return nullptr;
            if (text->getTyped(curPos)->code == 205) // )
                function->returnValue = TypesManager::Void();
            else
            {
                ReportError(text->getTyped(curPos), "Expected function return value");
                return nullptr;
            }
        }
        if (ThrowIfEndOfFile())
            return nullptr;
        brackets = text->getTyped(curPos++);
        bracketsError = brackets->code != 205; // )
    }
    if (bracketsError)
    {
        ReportError(brackets, "Expected function return type surrounded by ()");
        return nullptr;
    }
    
    // get function params
    if (IsEnd() || text->getTyped(curPos++)->code != 204) // (
    {
        ReportError(text->getTyped(curPos - 1), "function parameters must be surrounded by ()");
        return nullptr;
    }
    
    if (ThrowIfEndOfFile())
        return nullptr;
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
            
            if (ThrowIfEndOfFile())
                return nullptr;
            LexemeWord *next_word = text->getTyped(curPos++);
            if (next_word->code == 205) // )
                break;
            if (next_word->code != 242) // ,
            {
                ReportError(next_word, "Expected ',' between function parameters types");
                return nullptr;
            }
            if (ThrowIfEndOfFile())
                return nullptr;
        }
    }
    else
        curPos++;
    return result;
}

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    TNode *node = GetTLeaf(word, hasLeft, expectedRight);
    if (ErrorReported())
        return nullptr;
    node->parent = cur;
    cur->children.add(node);
    return cur;
}