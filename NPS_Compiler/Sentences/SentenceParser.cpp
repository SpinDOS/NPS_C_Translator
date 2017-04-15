//
// Created by Alexander on 25-Mar-17.
//

#include <iostream>
#include "SentenceParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../Operations/CustomOperationsManager.h"

TBranch *HandleKeyword(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{ReportError(word->positionInTheText, "No keywords support"); return 0;}

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    TNode *node = GetTLeaf(word, hasLeft, expectedRight);
    if (ErrorReported())
        return nullptr;
    node->parent = cur;
    cur->children.add(node);
    return cur;
}

TBranch *SentenceParser::HandleFunctionCall(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(word->positionInTheText, "Operation expected");
        return nullptr;
    }
    if (text->getTyped(curPos++)->code != 204) // (
    {
        ReportError(word->positionInTheText, "Expected '('");
        return nullptr;
    }
    
    // handle tnode
    TFunction *function = new TFunction;
    function->lexeme = word;
    function->parent = cur;
    cur->children.add(function);
    hasLeft = true;
    expectedRight = false;
    
    
    // parse arguments
    while (true)
    {
        TNode *arg = HandleExpression(true);
        if (ErrorReported())
            return nullptr;
        if (arg == nullptr)
            break;
        arg->parent = function;
        function->children.add(arg);
        if (text->getTyped(curPos)->code != 242) // ,
            break;
        curPos++;
    }
    
    if (text->getTyped(curPos++)->code != 205) // )
    {
        ReportError(word->positionInTheText, "Expected ')'");
        return nullptr;
    }
    return cur;
}

TBranch *SentenceParser::HandleOperation(TBranch *cur, LexemeWord *word,
                         bool &hasLeft, bool &expectedRight, bool stopOnComma)
{
    TOperation *operation = GetTOperation(word, hasLeft, expectedRight);
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
            ReportError(word->positionInTheText, "Invalid using ':' without '?'");
            return nullptr;
        }
        Heap::free_mem(cur->lexeme->lexeme);
        cur->lexeme->lexeme = copy_string("?:");
        cur->lexeme->code = 240;
        return cur;
    }
    
    while (operation->Priority > cur->Priority
           || (operation->Priority == cur->Priority && cur->IsLeftAssociated))
        if (cur->lexeme->code == 239) // ?
        {
            ReportError(word->positionInTheText, "Invalid using operator inside '?:' block");
            return nullptr;
        }
        else
            cur = cur->parent;
    
    // handle on root
    if (cur->parent == nullptr && (!expectedRight // ; ) ]
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
            ReportError(word->positionInTheText, "Expected ')'");
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
            ReportError(word->positionInTheText, "Expected ']'");
            return nullptr;
        }
        // change characteristics of the [
        cur->Priority = MINPRIORITY;
        cur->IsLeftAssociated = true;
        Heap::free_mem(cur->lexeme->lexeme);
        cur->lexeme->lexeme = copy_string("[]");
        return cur;
    }
    
    return nullptr;
}

TNode *SentenceParser::HandleExpression(bool stopOnComma)
{
    TOperation root;
    root.Priority = 10000;
    TBranch *cur = &root;
    bool hasLeft = false, expectedRight = false;
    while (!ErrorReported() && cur != nullptr)
    {
        LexemeWord *word = text->getTyped(curPos++);
        if (100 <= word->code && word->code < 200) // constant
            cur = HandleTLeaf(cur, word, hasLeft, expectedRight);
        else if (300 <= word->code && word->code < 400)// keyword
            cur = HandleKeyword(cur, word, hasLeft, expectedRight);
        else if (400 <= word->code && word->code < 600)// varname
        {
            if (TypesManager::GetTypeInfo(*word)) // type declaration
                ReportError(word->positionInTheText, "Unexpected type declaration");
            else if (CustomOperationsManager::IsFunctionExists(*word)) // function
                cur = HandleFunctionCall(cur, word, hasLeft, expectedRight);
            else
                cur = HandleTLeaf(cur, word, hasLeft, expectedRight); // variable
        }
        else // operation
            cur = HandleOperation(cur, word, hasLeft, expectedRight, stopOnComma);
    }
    if (ErrorReported() || root.children.count() == 0)
        return nullptr;
    TNode *result = root.children.takeFirst();
    result->parent = nullptr;
    return result;
}
