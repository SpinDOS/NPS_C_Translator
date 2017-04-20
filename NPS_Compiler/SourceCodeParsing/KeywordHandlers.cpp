//
// Created by Alexander on 18-Apr-17.
//

#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "iostream"


TNode* SourceCodeParser::GetConditionInBrackets()
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

TNode *SourceCodeParser::HandleKeywordDoWhile()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos++)); // get do
    TNode *body = ParseNextSentence(false); // get body
    if (ErrorReported())
        return nullptr;
    if (body != nullptr)
        body->parent = result;

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
    result->children.add(body);

    // validate ; after while
    lexeme = text->getTyped(curPos++);
    if (lexeme->code != 243) // ;
    {
        ReportError(lexeme, "Expected ';' after 'do .. while'");
        return nullptr;
    }
    return result;
}

TNode *SourceCodeParser::HandleKeywordWhile()
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
    TNode *initialization;
    if (TypesManager::IsType(text->getTyped(curPos)->lexeme))
    {
        initialization = HandleDeclaration();
        curPos--; // return back to ;
    }
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

TNode *SourceCodeParser::HandleKeywordIf()
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
    {
        result->children.add(nullptr); // fictive else
        return result;
    }
    curPos++;
    TNode *elseBody = ParseNextSentence(false);
    if (ErrorReported())
        return nullptr;

    if (elseBody != nullptr)
        elseBody->parent = result;
    result->children.add(elseBody);

    return result;
}

TNode *SourceCodeParser::HandleKeywordSwitch()
{
    TKeyword *result = new TKeyword(text->getTyped(curPos)); // get switch
    TNode *condition = GetConditionInBrackets();
    if (condition == nullptr)
        return nullptr;
    condition->parent = result;
    result->children.add(condition);
    LexemeWord *lexeme = text->getTyped(curPos++);
    if (lexeme->code != 200) // {
    {
        ReportError(lexeme, "Expected '{' after switch");
        return nullptr;
    }
    // create body of switch
    TList *body = new TList(lexeme);
    body->parent = result;
    result->children.add(body);

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
            switchCase->parent = result;
            if (lexeme->code == 307)
                switchCase->isDefault = true;
            else
            {
                lexeme = text->getTyped(curPos++);
                // get case number
                if (lexeme->code < 120 || lexeme->code >= 150) // not numeric constant
                {
                    ReportError(lexeme, "Numeric constant expected");
                    return nullptr;
                }
                char *type;
                double num = parse_num_constant(*lexeme, &type);
                if (strcmp(type, "double") == 0)
                {
                    ReportError(lexeme, "Integer numeric constant expected");
                    return nullptr;
                }
                switchCase->caseNum = (int) num;
                Heap::free_mem(type);
            }
            // validate :
            lexeme = text->getTyped(curPos++);
            if (lexeme->code != 240) // :
            {
                ReportError(lexeme, "Expected ':'");
                return nullptr;
            }
            // validate unique case
            for (int i = 2; i < result->children.count(); i++)
                if (static_cast<TSwitchCase*>(result->children.get(i))
                        ->operator==(*switchCase))
                {
                    ReportError(switchCase->lexeme, "Multiple case definition");
                    return nullptr;
                }
            result->children.add(switchCase);
            lexeme = text->getTyped(curPos++);
        }

        --curPos;
        // handle body
        while (lexeme->code != 201 && lexeme->code != 302 && lexeme->code != 307) // } case default
        {
            TNode *sentence = ParseNextSentence(false);
            if (ErrorReported())
                return nullptr;
            lexeme = text->getTyped(curPos);
            if (sentence == nullptr)
                continue;
            lineNum++;
            sentence->parent = body;
            body->children.add(sentence);
        }
        curPos++;
    }

    // remove cases without handler
    while (result->children.count() > 2 &&
           static_cast<TSwitchCase*>(result->children.getLast())->lineNum == lineNum)
        delete result->children.takeLast();
    // remove empty body
    if (body->children.count() == 0)
        delete result->children.takeLast();
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
        expr->parent = result;
        result->children.add(expr);
    }
    LexemeWord *nextLex = text->getTyped(curPos++);
    if (nextLex->code == 243) // ;
        return result;
    ReportError(nextLex, "Expected ';' after 'return'");
    return nullptr;
}
