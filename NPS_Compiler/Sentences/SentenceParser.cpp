//
// Created by Alexander on 25-Mar-17.
//

#include <iostream>
#include "SentenceParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../Operations/CustomOperationsManager.h"

TBranch *HandleKeyword(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{ReportError(word->start, "No keywords support"); return 0;}

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    TNode *node = GetTLeaf(word, hasLeft, expectedRight);
    if (ErrorReported())
        return nullptr;
    node->parent = cur;
    cur->children.add(node);
    return cur;
}

TBranch *HandleFunction(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{ReportError(word->start, "No function support"); return 0;}

TBranch *SentenceParser::HandleOperation(TBranch *cur, LexemeWord *word,
                         bool &hasLeft, bool &expectedRight, bool stopOnComma)
{
    TOperation *operation = GetTOperation(word, hasLeft, expectedRight);
    if (ErrorReported())
    {
        delete operation;
        return nullptr;
    }
    
    if (word->code == 204 || word->code == 206) // ( [
    {
        if (word->code == 206)
            operation->children.add(cur->children.takeLast());
        operation->parent = cur;
        cur->children.add(operation);
        return operation;
    }
    
    // handle :
    if (word->code == 240) // :
    {
        while (operation->Priority > cur->Priority
               || (operation->Priority == cur->Priority && cur->lexeme->code != 239)) // ?
            cur = cur->parent;
        
        delete operation;
        if (cur->lexeme->code != 239) // ?
        {
            ReportError(word->start, "Invalid using ':' without '?'");
            return nullptr;
        }
        cur->lexeme->code = 240;
        return cur;
    }
    
    while (operation->Priority > cur->Priority
           || (operation->Priority == cur->Priority && cur->IsLeftAssociated))
        if (cur->lexeme->code == 239) // ?
        {
            delete operation;
            ReportError(word->start, "Invalid using operator inside '?:' block");
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
            ReportError(word->start, "Expected ')'");
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
            ReportError(word->start, "Expected ']'");
            return nullptr;
        }
        // change characteristics of the [
        cur->Priority = 0;
        cur->IsLeftAssociated = true;
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
                ReportError(word->start, "Unexpected type declaration");
            else if (CustomOperationsManager::IsOperationExists(*word)) // function
                cur = HandleFunction(cur, word, hasLeft, expectedRight);
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
