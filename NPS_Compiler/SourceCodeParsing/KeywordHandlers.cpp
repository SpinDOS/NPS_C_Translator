//
// Created by Alexander on 18-Apr-17.
//

#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "iostream"

TNode* SourceCodeParser::GetConditionInBrackets()
{
    // in: curpos is on keyword before brackets
    // out: curpos is after ')'
    std::string kword(text->getTyped(curPos++)->lexeme);
    LexemeWord *lexeme = text->getTyped(curPos++); // get (
    if (lexeme->code != 204) // (
    {
        ReportError(lexeme, ("Expected '(' after '" + kword + "'").c_str());
        return nullptr;
    }
    TNode *condition = HandleExpression(true);
    if (condition == nullptr)
    {
        if (!ErrorReported())
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

TNode *SourceCodeParser::HandleKeywordDoWhile()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++)); // get do
    TNode *body = ParseNextSentence(); // get body
    if (ThrowIfEndOfFile())
        return nullptr;
    if (body != nullptr)
        body->Parent = result;
    
    LexemeWord *lexeme = text->getTyped(curPos);
    if (lexeme->code != 330) // while
    {
        ReportError(lexeme, "Expected 'while' after 'do {body}'");
        return nullptr;
    }
    // get condition
    TNode *condition = GetConditionInBrackets();
    if (ErrorReported())
        return nullptr;
    condition->Parent = result;

    // validate ; after while
    lexeme = text->getTyped(curPos++);
    if (lexeme->code != 243) // ;
    {
        ReportError(lexeme, "Expected ';' after 'do .. while'");
        return nullptr;
    }
    result->Children.add(condition);
    result->Children.add(body);
    return result;
}

TNode *SourceCodeParser::HandleKeywordWhile()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get while
    // get condition
    TNode *condition = GetConditionInBrackets();
    if (ErrorReported())
        return nullptr;
    condition->Parent = result;
    result->Children.add(condition);
    
    TNode *body = ParseNextSentence(); // get body
    if (ThrowIfEndOfFile())
        return nullptr;
    if (body != nullptr)
        body->Parent = result;
    result->Children.add(body);
    return result;
}

TNode *SourceCodeParser::HandleKeywordFor()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++)); // get for
    LexemeWord *lexeme = text->getTyped(curPos++); // get (
    if (lexeme->code != 204) // (
    {
        ReportError(lexeme, "Expected '(' after 'for'");
        return nullptr;
    }
    
    // get initialization
    if (IsMeetType())
    {
        if (!GetDeclaration(result)) // get declarations
            return nullptr;
    }
    else if (ErrorReported())
        return nullptr;
    else
    {
        TNode *initialization = HandleExpression(false);
        if (ErrorReported())
            return nullptr;
        lexeme = text->getTyped(curPos++); // get ;
        if (lexeme->code != 243) // ;
        {
            ReportError(lexeme, "Expected ';' after 'for' initialization");
            return nullptr;
        }
        if (initialization != nullptr)
            initialization->Parent = result;
        result->Children.add(initialization);
    }
    
    // get condition segment
    if (ThrowIfEndOfFile())
        return nullptr;
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
        condition->Parent = result;
    result->Children.add(condition);

    // get post body
    if (ThrowIfEndOfFile())
        return nullptr;
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
        post_body->Parent = result;
    result->Children.add(post_body);

    // get body
    TNode *body = ParseNextSentence();
    if (ThrowIfEndOfFile())
        return nullptr;
    if (body != nullptr)
        body->Parent = result;
    result->Children.add(body);

    return result;
}

TNode *SourceCodeParser::HandleKeywordIf()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get if
    TNode *condition = GetConditionInBrackets();
    if (ErrorReported())
        return nullptr;
    condition->Parent = result;
    result->Children.add(condition);

    // handle if-body
    TNode *ifBody = ParseNextSentence();
    if (ThrowIfEndOfFile())
        return nullptr;
    if (ifBody != nullptr)
        ifBody->Parent = result;
    result->Children.add(ifBody);

    // handle else-body
    LexemeWord *lexeme = text->getTyped(curPos);
    if (lexeme->code != 310) // else
    {
        result->Children.add(nullptr); // fictive else
        return result;
    }
    curPos++;
    TNode *elseBody = ParseNextSentence();
    if (ThrowIfEndOfFile())
        return nullptr;
    if (elseBody != nullptr)
        elseBody->Parent = result;
    result->Children.add(elseBody);

    return result;
}

TNode *SourceCodeParser::HandleKeywordSwitch()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get switch
    
    TNode *condition = GetConditionInBrackets();
    if (ErrorReported())
        return nullptr;
    condition->Parent = result;
    result->Children.add(condition);
    LexemeWord *lexeme = text->getTyped(curPos++);
    if (lexeme->code != 200) // {
    {
        ReportError(lexeme, "Expected '{' after switch");
        return nullptr;
    }
    // create body of switch
    TList *body = new TList(lexeme);
    body->Parent = result;
    result->Children.add(body);

    lexeme = text->getTyped(curPos++);
    int lineNum = 0;
    // inspect whole switch
    while (lexeme->code != 201) // }
    {
        // make sure we meet case or default
        if (lexeme->code != 302 && lexeme->code != 307) // case default
        {
            ReportError(lexeme, "'case' or 'default' expected");
            return nullptr;
        }
        // handle labels
        while (lexeme->code == 302 || lexeme->code == 307) // case default
        {
            TSwitchCase *switchCase = new TSwitchCase(lexeme);
            switchCase->lineNum = lineNum;
            switchCase->Parent = result;
            if (lexeme->code == 307) // default
                switchCase->isDefault = true;
            else
            {
                lexeme = text->getTyped(curPos++);
                double num;
                // get case number
                if (lexeme->code == 110) // char constant
                    num = parse_char_constant(*lexeme);
                else
                {
                    if (lexeme->code < 120 || lexeme->code >= 150) // not numeric constant
                    {
                        ReportError(lexeme, "Numeric constant expected");
                        return nullptr;
                    }
                    char *type;
                    num = parse_num_constant(*lexeme, &type);
                    if (strcmp(type, "double") == 0)
                    {
                        ReportError(lexeme, "Integer numeric constant expected");
                        return nullptr;
                    }
                    Heap::free_mem(type);
                }
                
                switchCase->caseNum = (int) num;
            }
            
            // validate :
            lexeme = text->getTyped(curPos++);
            if (lexeme->code != 240) // :
            {
                ReportError(lexeme, "Expected ':'");
                return nullptr;
            }
            // validate unique case
            for (int i = 2; i < result->Children.count(); i++)
                if (static_cast<TSwitchCase*>(result->Children.get(i))
                        ->operator==(*switchCase))
                {
                    ReportError(switchCase->Lexeme, "Multiple case definition");
                    return nullptr;
                }
            result->Children.add(switchCase);
            lexeme = text->getTyped(curPos++);
        }

        --curPos;
        // handle body
        while (lexeme->code != 201 && lexeme->code != 302 && lexeme->code != 307) // } case default
        {
            TNode *sentence = ParseNextSentence();
            if (ThrowIfEndOfFile())
                return nullptr;
            lexeme = text->getTyped(curPos);
            if (sentence == nullptr)
                continue;
            lineNum++;
            sentence->Parent = body;
            body->Children.add(sentence);
        }
        curPos++;
    }

    // remove cases without handler
    while (result->Children.count() > 2 &&
           static_cast<TSwitchCase*>(result->Children.getLast())->lineNum == lineNum)
        delete result->Children.takeLast();
    // remove empty body
    if (body->Children.count() == 0)
    {
        delete result->Children.takeLast();
        result->Children.add(nullptr);
    }
    return result;
}

TNode *SourceCodeParser::HandleKeywordBreakContinue()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++));
    LexemeWord *nextLex = text->getTyped(curPos++);
    if (nextLex->code == 243) // ;
        return result;

    ReportError(nextLex, "Expected ';' after 'break' or 'continue'");
    return nullptr;
}

TNode *SourceCodeParser::HandleKeywordReturn()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++));
    TNode *expr = HandleExpression(false);
    if (ErrorReported())
        return nullptr;
    if (expr != nullptr)
    {
        expr->Parent = result;
        result->Children.add(expr);
    }
    LexemeWord *nextLex = text->getTyped(curPos++);
    if (nextLex->code == 243) // ;
        return result;
    ReportError(nextLex, "Expected ';' after 'return'");
    return nullptr;
}

TNode* SourceCodeParser::HandleKeywordDelete()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++));
    result->Children.add(HandleExpression(false));
    if (ErrorReported())
        return nullptr;
    if (result->Children.getFirst() == nullptr)
    {
        ReportError(text->getTyped(curPos - 1), "Expected expression to delete");
        return nullptr;
    }
    LexemeWord *nextLex = text->getTyped(curPos++);
    if (nextLex->code == 243) // ;
        return result;
    ReportError(nextLex, "Expected ';' after 'delete'");
    return nullptr;
}

TNode* SourceCodeParser::HandleKeywordNew()
{
    TKeywordNew *result = new TKeywordNew(text->getTyped(curPos++));
    result->DeclaringType = TryGetResultType();
    if (result->DeclaringType == nullptr)
    {
        if (!ErrorReported())
            ReportError(text->getTyped(curPos - 1), "Expected type to allocate memory");
        return nullptr;
    }
    LexemeWord *lexeme = text->getTyped(curPos++);
    if (lexeme->code == 206) // [
    {
        result->Array_length = GetArrayBrackets();
        if (ErrorReported())
            return nullptr;
        lexeme = text->getTyped(curPos++);
    }
    if (lexeme->code == 204) // (
    {
        if (!GetParametersInBrackets(result))
            return nullptr;
        lexeme = text->getTyped(curPos++);
    }
    
    if (lexeme->code == 243) // ;
        return result;
    
    ReportError(lexeme, "Expected ';'");
    return nullptr;
}