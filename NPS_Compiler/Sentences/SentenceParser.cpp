//
// Created by Alexander on 25-Mar-17.
//

#include <iostream>
#include "SentenceParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../Operations/CustomOperationsManager.h"

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    TNode *node = GetTLeaf(word, hasLeft, expectedRight);
    if (ErrorReported())
        return nullptr;
    node->parent = cur;
    cur->children.add(node);
    return cur;
}

SentenceParser::SentenceParser(TypeList<LexemeWord> *words)
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

TBranch *SentenceParser::HandleFunctionCall(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(word, "Unexpected funtion call. Operation expected");
        return nullptr;
    }
    if (text->getTyped(curPos++)->code != 204) // (
    {
        ReportError(word, "Expected '('");
        return nullptr;
    }
    
    // handle tnode
    TFunction *function = new TFunction(word);
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
        ReportError(word, "Expected ')'");
        return nullptr;
    }
    return cur;
}

TNode* SentenceParser::HandleDeclaration()
{
    const char *type = text->getTyped(curPos++)->lexeme;
    LexemeWord rootLexeme;
    rootLexeme.code = 200;
    TOperation root(&rootLexeme);
    TBranch *cur = &root;
    while (true)
    {
        // get all **
        int p_count = 0;
        while (text->getTyped(curPos)->code == 218) // *
        {
            p_count++;
            curPos++;
        }
        
        // check for var name
        LexemeWord *var = text->getTyped(curPos++);
        if (var->code < 400 || var->code >= 600 || // not varname
            TypesManager::GetTypeInfo(var->lexeme) || // second type declaration
            CustomOperationsManager::IsFunctionExists(var->lexeme)) // function call
        {
            ReportError(var, "Expected identifier");
            return nullptr;
        }
        
        if (p_count == 0 && strcmp(type, "void") == 0)
        {
            ReportError(var, "Declaring variables of type void is not allowed");
            return nullptr;
        }
        
        // create tnode
        TDeclaration *declaration = new TDeclaration(var);
        declaration->parent = cur;
        declaration->arrayLength = nullptr;
        
        // parse declaration / initialization until , or ;
        if (text->getTyped(curPos)->code == 241) // =
        {
            --curPos;
            TBranch *initialization = static_cast<TBranch*>(HandleExpression(true));
            if (ErrorReported())
                return nullptr;
            // replace variable name with declaration under root '='
            delete initialization->children.takeFirst();
            initialization->children.insertBefore(declaration, 0);
            declaration->parent = initialization;
            
            cur->children.add(initialization);
            initialization->parent = cur;
        }
        else
        {
            cur->children.add(declaration);
            if (text->getTyped(curPos)->code == 206) // [
            {
                LexemeWord *arrayLengthStart = text->getTyped(++curPos);
                declaration->arrayLength = HandleExpression(false);
                declaration->arrayLength->parent = nullptr;
                p_count++;
                if (declaration->arrayLength == nullptr)
                {
                    if (!ErrorReported())
                        ReportError(arrayLengthStart, "Array length expected");
                    return nullptr;
                }
                if (text->getTyped(curPos++)->code != 207) // ]
                {
                    if (!ErrorReported())
                        ReportError(arrayLengthStart, "Expected ']'");
                    return nullptr;
                }
            }
        }
        // laze type initialization for array declaration
        declaration->type = new ResultType(type, p_count, false);
        
        if (text->getTyped(curPos)->code == 243) // ;
            return root.children.takeFirst();
        if (text->getTyped(curPos)->code != 242) // ,
        {
            ReportError(text->getTyped(curPos), "Expected ',' or ';'");
            return nullptr;
        }
        // handle ,
        bool temp1 = true, temp2 = false;
        TOperation *comma = GetTOperation(text->getTyped(curPos++), temp1, temp2);
        comma->children.add(cur->children.takeLast());
        comma->children.getLast()->parent = comma;
        cur->children.add(comma);
        comma->parent = cur;
        cur = comma;
    }
    return nullptr;
}

TOperation* SentenceParser::GetTypeCast(LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(word, "Unexpected type cast. Operation expected");
        return nullptr;
    }
    // get info about target type
    LexemeWord *targetType = text->getTyped(curPos);
    int p_count = 0;
    while (text->getTyped(++curPos)->code == 218) // *
        p_count++;

    // check for terminal )
    LexemeWord *lastLexeme = text->getTyped(curPos++);
    if (lastLexeme->code != 205) // )
    {
        ReportError(lastLexeme, "Expected ')'");
        return nullptr;
    }
    hasLeft = false;
    expectedRight = true;
    return new TTypeCast(targetType, p_count);
}

TBranch *SentenceParser::HandleOperation(TBranch *cur, LexemeWord *word,
                                         bool &hasLeft, bool &expectedRight, bool stopOnComma)
{
    TOperation *operation;
    if (word->code == 204 && // (
        TypesManager::GetTypeInfo(*text->getTyped(curPos)) != nullptr)
    {
        operation = GetTypeCast(word, hasLeft, expectedRight);
        if (operation != nullptr)
            word = operation->lexeme;
    }
    else
        operation= GetTOperation(word, hasLeft, expectedRight);
    
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
    
    while (operation->Priority > cur->Priority
           || (operation->Priority == cur->Priority && cur->IsLeftAssociated))
        if (cur->lexeme->code == 239) // ?
        {
            ReportError(word, "Invalid using operator inside '?:' block");
            return nullptr;
        }
        else
            cur = cur->parent;
    
    // handle on root
    if (cur->parent == nullptr &&
            (word->code == 243 || word->code == 205 || word->code == 207) // ; ) ]
             || (stopOnComma && word->code == 242)) // ,
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
    LexemeWord rootLexeme;
    rootLexeme.code = 200;
    TOperation root(&rootLexeme);
    root.Priority = 10000;
    TBranch *cur = &root;
    bool hasLeft = false, expectedRight = false;
    while (!ErrorReported() && cur != nullptr)
    {
        LexemeWord *word = text->getTyped(curPos++);
        if (100 <= word->code && word->code < 200) // constant
            cur = HandleTLeaf(cur, word, hasLeft, expectedRight);
        else if (300 <= word->code && word->code < 400)// keyword
        {
            // new, delete keywords here
            ReportError(word, "Unexpected keyword. Maybe you miss ';'?");
        }
        else if (400 <= word->code && word->code < 600)// varname
        {
            if (TypesManager::GetTypeInfo(*word)) // type declaration
                ReportError(word, "Unexpected type declaration. Maybe you miss ';'?");
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

TNode* SentenceParser::ParseNextSentence(bool declarationAllowed)
{
    LexemeWord *word = text->getTyped(curPos);
    if (300 <= word->code && word->code < 400)// keyword
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
            case 318: // new
                //return HandleExpression(false);
                ReportError(word, "This keyword is not implemented yet");
                return nullptr;
            default:
                ReportError(word, "This keyword can not be used here");
                return nullptr;
                
        }
    else if (400 <= word->code && word->code < 600
             && TypesManager::GetTypeInfo(*word))// type declaration
    {
        if (declarationAllowed)
            return HandleDeclaration();
        ReportError(word, "Type declaration is not allowed here");
        return nullptr;
    }
    else if (word->code == 200) // {
        return ParseList();
    else
    {
        TNode *result = HandleExpression(false);
        if (ErrorReported())
            return nullptr;
        
        // validate terminating ;
        word = text->getTyped(curPos++);
        if (word->code != 243) // ;
        {
            ReportError(word, "Expected ';' ");
            return nullptr;
        }
        if (result == nullptr)
            return nullptr;
        
        // check for a;
        if (result->tNodeType == TNodeTypeConstant ||
            result->tNodeType == TNodeTypeVariable)
        {
            ReportError(result->lexeme, "Unexpected operand. Operation expected");
            return nullptr;
        }
        return result;
    }
    return nullptr;
}

TList* SentenceParser::ParseList()
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

TList* SentenceParser::ParseWholeText()
{
    // TEMPORARY FIX
    // add fictive {} to easily detect end of file without error
    LexemeWord *lastLexeme = text->getTyped(text->count() - 1);
    LexemeWord *lexeme = static_cast<LexemeWord*>(Heap::get_mem(sizeof(LexemeWord)));
    lexeme->code = 201;
    lexeme->positionInTheText = lastLexeme->positionInTheText + strlen(lastLexeme->lexeme);
    lexeme->lexeme = copy_string("}");
    text->add(lexeme);
    TList *result = ParseList();
    return result? (TList*) result->children.takeLast() : nullptr;
}

TNode* SentenceParser::GetConditionInBrackets()
{
    std::string kword(text->getTyped(curPos++)->lexeme);
    LexemeWord *lexeme = text->getTyped(curPos++); // get (
    if (lexeme->code != 204) // (
    {
        ReportError(lexeme, ("Expected '(' after '" + kword + "'").c_str());
        return nullptr;
    }
    TNode *condition = HandleExpression(true);
    if (ErrorReported())
        return nullptr;
    if (condition == nullptr)
    {
        ReportError(lexeme->positionInTheText + 1, ("Condition expected after '" + kword + "'").c_str());
        return nullptr;
    }
    lexeme = text->getTyped(curPos++); // get )
    if (lexeme->code != 205) // )
    {
        ReportError(lexeme, ("Expected ')' after condition of the '" + kword + "'").c_str());
        return nullptr;
    }
    return condition;
}

TNode *SentenceParser::HandleKeywordDoWhile()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++)); // get do
    TNode *body = ParseNextSentence(false); // get body
    if (ErrorReported())
        return nullptr;
    if (body != nullptr)
        body->parent = result;
    result->children.add(body);
    
    LexemeWord *lexeme = text->getTyped(curPos);
    if (lexeme->code != 330) // while
    {
        ReportError(lexeme, "Expected 'while' after 'do {body}'");
        return nullptr;
    }
    // get condition
    TNode *condition = GetConditionInBrackets();
    if (condition == nullptr)
        return nullptr;
    condition->parent = result;
    result->children.add(condition);
    
    // validate ; after while
    lexeme = text->getTyped(curPos++);
    if (lexeme->code != 243) // ;
    {
        ReportError(lexeme, "Expected ';' after 'do .. while'");
        return nullptr;
    }
    return result;
}

TNode *SentenceParser::HandleKeywordWhile()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get while
    // get condition
    TNode *condition = GetConditionInBrackets();
    if (condition == nullptr)
        return nullptr;
    condition->parent = result;
    result->children.add(condition);
    
    TNode *body = ParseNextSentence(false); // get body
    if (ErrorReported())
        return nullptr;
    if (body != nullptr)
        body->parent = result;
    result->children.add(body);
    
    return result;
}

TNode *SentenceParser::HandleKeywordFor()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++)); // get for
    LexemeWord *lexeme = text->getTyped(curPos++); // get (
    if (lexeme->code != 204) // (
    {
        ReportError(lexeme, "Expected '(' after 'for'");
        return nullptr;
    }
    
    // get initialization
    TNode *initialization;
    if (TypesManager::GetTypeInfo(text->getTyped(curPos)->lexeme) != nullptr)
        initialization = HandleDeclaration();
    else
        initialization = HandleExpression(false);
    if (ErrorReported())
        return nullptr;
    lexeme = text->getTyped(curPos++); // get ;
    if (lexeme->code != 243) // ;
    {
        ReportError(lexeme, "Expected ';' after 'for' initialization");
        return nullptr;
    }
    if (initialization != nullptr)
        initialization->parent = result;
    result->children.add(initialization);
    
    // get condition segment
    TNode *condition = HandleExpression(true);
    if (ErrorReported())
        return nullptr;
    lexeme = text->getTyped(curPos++); // get ;
    if (lexeme->code != 243) // ;
    {
        ReportError(lexeme, "Expected ';' after 'for' condition");
        return nullptr;
    }
    if (condition != nullptr)
        condition->parent = result;
    result->children.add(condition);
    
    // get post body
    TNode *post_body = HandleExpression(false);
    if (ErrorReported())
        return nullptr;
    lexeme = text->getTyped(curPos++); // get )
    if (lexeme->code != 205) // )
    {
        ReportError(lexeme, "Expected ')' after 'for' post body block");
        return nullptr;
    }
    if (post_body != nullptr)
        post_body->parent = result;
    result->children.add(post_body);
    
    // get body
    TNode *body = ParseNextSentence(false);
    if (ErrorReported())
        return nullptr;
    if (body != nullptr)
        body->parent = result;
    result->children.add(body);
    
    return result;
}

TNode *SentenceParser::HandleKeywordIf()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get if
    TNode *condition = GetConditionInBrackets();
    if (condition == nullptr)
        return nullptr;
    condition->parent = result;
    result->children.add(condition);
    
    // handle if-body
    TNode *ifBody = ParseNextSentence(false);
    if (ErrorReported())
        return nullptr;
    if (ifBody != nullptr)
        ifBody->parent = result;
    result->children.add(ifBody);
    
    // handle else-body
    LexemeWord *lexeme = text->getTyped(curPos);
    if (lexeme->code != 310) // else
        return result;
    curPos++;
    TNode *elseBody = ParseNextSentence(false);
    if (ErrorReported())
        return nullptr;
    
    if (elseBody != nullptr)
        elseBody->parent = result;
    result->children.add(elseBody);
    
    return result;
}

TNode *SentenceParser::HandleKeywordSwitch()
{
    ReportError(text->getTyped(curPos), "Switch is not implemented yet");
    return nullptr;
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get switch
    TNode *condition = GetConditionInBrackets();
    if (condition == nullptr)
        return nullptr;
    condition->parent = result;
    result->children.add(condition);
    
    
    
    return result;
}

TNode *SentenceParser::HandleKeywordBreakContinue()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++));
    LexemeWord *nextLex = text->getTyped(curPos++);
    if (nextLex->code == 243) // ;
        return result;
    
    ReportError(nextLex, "Expected ';' after 'break' or 'continue'");
    return nullptr;
}

TNode *SentenceParser::HandleKeywordReturn()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++));
    TNode *expr = HandleExpression(false);
    if (ErrorReported())
        return nullptr;
    
    if (expr != nullptr)
    {
        expr->parent = result;
        result->children.add(expr);
    }
    LexemeWord *nextLex = text->getTyped(curPos++);
    if (nextLex->code == 243) // ;
        return result;
    ReportError(nextLex, "Expected ';' after 'return'");
    return nullptr;
}
