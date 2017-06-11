//
// Created by Alexander on 18-Apr-17.
//

#include "SourceCodeParser.h"

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight);


TNode* SourceCodeParser::ParseNextSentence()
{
    // in: curpos is on first sentence symbol
    // out: curpos is after ';'
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
            case 314: // base
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
    word = text->getTyped(curPos++);
    if (word->code != 243) // ;
    {
        ReportError(word, "Expected ';'");
        return nullptr;
    }
    if (result == nullptr)
        return nullptr;
    
    // check for a;
    if (dynamic_cast<TLeaf*>(result) != nullptr)
    {
        ReportError(result->Lexeme, "Sentence can not contain only constant or variable");
        return nullptr;
    }
    return result;
}

TNode* SourceCodeParser::HandleExpression(bool stopOnComma)
{
    // in: curpos is on first expression symbol
    // out: curpos is on ';' or smth like it
    TFictiveRoot root;
    TBranch *cur = &root;
    bool hasLeft = false, expectedRight = false;
    while (!ErrorReported() && cur != nullptr)
    {
        LexemeWord *word = text->getTyped(curPos++);
        if (100 <= word->code && word->code < 200) // constant
            cur = HandleTLeaf(cur, word, hasLeft, expectedRight);
        else if (300 <= word->code && word->code < 400)// keyword
        {
            TNode *newNode;
            switch (word->code)
            {
                case 318: // new
                    newNode = HandleKeywordNew();
                    break;
                case 300: // null
                    newNode = new TConstant(word);
                    break;
                case 314: // base
                case 315: // this
                    newNode = new TVariable(word);
                    break;
                default:
                    ReportError(word, "Unexpected keyword. Maybe you miss ';'?");
                    return nullptr;
            }
            if (ErrorReported())
                return nullptr;
            // dont change cur
            newNode->Parent = cur;
            cur->Children.add(newNode);
            expectedRight = false;
            hasLeft = true;
        }
        else if (400 <= word->code && word->code < 600)// varname
        {
            if (IsMeetType()) // type declaration
            {
                ReportError(word, "Unexpected type declaration. Maybe you miss ';'?");
                return nullptr;
            }
            if (ErrorReported())
                return nullptr;
            cur = HandleTLeaf(cur, word, hasLeft, expectedRight); // variable
        }
        else if (hasLeft && word->code == 204) // (
            cur = HandleFunctionCall(cur, word, hasLeft, expectedRight); // function call
        else // operation
            cur = HandleOperation(cur, word, hasLeft, expectedRight, stopOnComma);
    }
    if (ErrorReported() || root.Children.count() == 0)
        return nullptr;
    TNode *result = root.Children.takeFirst();
    result->Parent = nullptr;
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
               || (operation->Priority == cur->Priority && cur->Lexeme->code != 239)) // ?
            cur = cur->Parent;
        
        delete operation;
        if (cur->Lexeme->code != 239) // ?
        {
            ReportError(word, "Invalid using ':' without '?'");
            return nullptr;
        }
        Heap::free_mem(cur->Lexeme->lexeme);
        cur->Lexeme->lexeme = copy_string("?:");
        cur->Lexeme->code = 240;
        return cur;
    }
    
    while (operation->Priority > cur->Priority ||
           (operation->Priority == cur->Priority && cur->IsLeftAssociated))
        if (cur->Lexeme->code == 239) // ?
        {
            ReportError(word, "Invalid using operator inside '?:' block");
            return nullptr;
        }
        else
            cur = cur->Parent;
    
    // handle on root
    if (cur->Parent == nullptr &&
        ((operation->Priority == MAXPRIORITY && dynamic_cast<TTypeCast*>(operation) == nullptr) // ; ] )
         || (stopOnComma && word->code == 242))) // ,
    {
        curPos--;
        delete operation;
        return nullptr;
    }
    
    // simple operation
    if (cur->Priority != operation->Priority ||
        (cur->Lexeme->code != 204 && cur->Lexeme->code != 206)) // ( [
    {
        if (word->code == 204 || word->code == 206) // ( [
            operation->Priority = MAXPRIORITY;
        
        if (operation->IsLeftAssociated || operation->NumOfChildren > 1)
        {
            operation->Children.add(cur->Children.takeLast());
            operation->Children.getLast()->Parent = operation;
        }
        cur->Children.add(operation);
        operation->Parent = cur;
        return operation;
    }
    
    // handle ()
    if (cur->Lexeme->code == 204) // (
    {
        int code = operation->Lexeme->code;
        delete operation;
        if (code != 205) // )
        {
            ReportError(word, "Expected ')'");
            return nullptr;
        }
        TBranch *parent = cur->Parent;
        parent->Children.takeLast();
        parent->Children.add(cur->Children.takeFirst());
        parent->Children.getLast()->Parent = parent;
        delete cur;
        return parent;
    }
    
    // handle []
    if (cur->Lexeme->code == 206) // [
    {
        int code = operation->Lexeme->code;
        delete operation;
        if (code != 207) // ]
        {
            ReportError(word, "Expected ']'");
            return nullptr;
        }
        Heap::free_mem(cur->Lexeme->lexeme);
        cur->Lexeme->lexeme = copy_string("[]");
        return cur->Parent;
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
    ResultType *targetType = TryGetResultType();
    if (targetType == nullptr)
    {
        if (!ErrorReported())
            ReportError(text->getTyped(curPos - 1), "Expected type to cast");
        return nullptr;
    }
    
    // check for terminal )
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
    
    TFunctionCall *function = new TFunctionCall(word);
    function->Parent = cur;
    function->FunctionToCall = cur->Children.takeLast();
    function->FunctionToCall->Parent = function;
    cur->Children.add(function);
    
    if (!GetParametersInBrackets(function))
        return nullptr;
    
    function->Lexeme->code = 205;
    Heap::free_mem(function->Lexeme->lexeme);
    function->Lexeme->lexeme = copy_string("()");
    hasLeft = true;
    expectedRight = false;
    return cur;
}


TList* SourceCodeParser::ParseList()
{
    // in: curpos is on {
    // out: curpos is after }
    TList *list = new TList(text->getTyped(curPos++));
    while (true)
    {
        LexemeWord *word = text->getTyped(curPos);
        
        if (word->code == 201) // }
            break;
        
        if (IsMeetType())
        { // type declaration
            if (!GetDeclaration(list))
                return nullptr;
            continue;
        }
        if (ErrorReported())
            return nullptr;
        
        TNode *sentence = ParseNextSentence();
        if (ThrowIfEndOfFile())
            return nullptr;
        if (sentence == nullptr)
            continue;
        if (dynamic_cast<TLeaf*>(sentence) != nullptr)
        {
            ReportError(sentence->Lexeme, "Sentence can not consist of variable or constant");
            return nullptr;
        }
        sentence->Parent = list;
        list->Children.add(sentence);
    }
    curPos++;
    return list;
}



TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    TNode *node = GetTLeaf(word, hasLeft, expectedRight);
    if (ErrorReported())
        return nullptr;
    node->Parent = cur;
    cur->Children.add(node);
    return cur;
}