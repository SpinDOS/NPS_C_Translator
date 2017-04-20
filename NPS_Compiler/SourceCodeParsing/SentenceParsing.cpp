//
// Created by Alexander on 18-Apr-17.
//

#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

bool IsValidVarName(const char *var)
{ return TypesManager::IsType(var) == nullptr; }

TBranch *HandleTLeaf(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    TNode *node = GetTLeaf(word, hasLeft, expectedRight);
    if (ErrorReported())
        return nullptr;
    node->parent = cur;
    cur->children.add(node);
    return cur;
}

ResultType* SourceCodeParser::GetDeclaringType(TSimpleLinkedList<LexemeWord> *parameters)
{
    LexemeWord *baseType = text->getTyped(curPos++);
    ResultType *resultType = new ResultType;
    while (text->getTyped(curPos)->code == 218)
        resultType->p_count++, curPos++;

    if (strcmp(baseType->lexeme, "function") != 0)
    {
        if (resultType->p_count == 0 && strcmp(baseType->lexeme, "void") == 0)
        {
            ReportError(text->getTyped(curPos),
                        "Declaring variables of type void is not allowed");
            return nullptr;
        }
        if (TypesManager::IsPrimitive(baseType->lexeme))
            resultType->baseType = new PrimitiveType;
        else
            resultType->baseType = new CustomType;
        static_cast<VarType*>(resultType->baseType)->type = copy_string(baseType->lexeme);
        return resultType;
    }
    Func *function = new Func;
    resultType->baseType = function;

    // get function signature
    bool bracketsError = true;
    LexemeWord *word = text->getTyped(curPos++);
    // get function return value
    if (word->code == 204) // (
    {
        function->returnValue = GetDeclaringType();
        if (ErrorReported())
            return nullptr;
        if (function->returnValue == nullptr)
        {
            ReportError(word, "function return value can not be empty. You can use void");
            return nullptr;
        }
        bracketsError = text->getTyped(curPos++)->code != 205; // )
    }
    if (bracketsError)
    {
        ReportError(word, "funtion return type must be surrounded by ()");
        return nullptr;
    }

    // get function params
    bracketsError = true;
    if (word->code == 204) // (
    {
        if (text->getTyped(curPos++)->code == 205) // )
            return resultType;
        do
        {
            ResultType *paramType = GetDeclaringType();
            if (ErrorReported())
                return nullptr;
            if (paramType == nullptr)
            {
                ReportError(text->getTyped(curPos), "Empty function parameter type");
                return nullptr;
            }
            function->parameters.add(paramType);

            // get parameter name
            LexemeWord *paramName = text->getTyped(curPos);
            if (400 <= paramName->code && paramName->code < 600) // varname
            {
                if (!IsValidVarName(paramName->lexeme))
                {
                    ReportError(paramName, "function parameter name expected, not type");
                    return nullptr;
                }
                curPos++;
                if (parameters != nullptr) // if needed to provide info about parameter names
                {
                    parameters->add(paramName);
                    continue;
                }
            }
            if (parameters != nullptr)
            {
                ReportError(paramName, "Missing parameter name");
                return nullptr;
            }
        }
        while (text->getTyped(curPos++)->code == 242); // ,
        bracketsError = text->getTyped(curPos - 1)->code != 205; // )
    }
    if (bracketsError)
    {
        ReportError(word, "function parameters must be surrounded by ()");
        return nullptr;
    }
    return resultType;
}

TBranch *SourceCodeParser::HandleFunctionCall(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    // handle tnode
    TFunction *function = new TFunction(word);
    function->parent = cur;
    function->children.add(cur->children.takeLast());
    cur->children.add(function);

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
    Heap::free_mem(function->lexeme->lexeme);
    function->lexeme->lexeme = copy_string("()");
    hasLeft = true;
    expectedRight = false;
    return cur;
}

TNode* SourceCodeParser::HandleDeclaration()
{
    LexemeWord temp;
    // FOR CLASSES SKIP . AND :: HERE
    LexemeWord *type = text->getTyped(curPos);
    LexemeWord rootLexeme;
    rootLexeme.code = 200;
    TList root(&rootLexeme);
    TBranch *cur = &root;
    while (true)
    {
        // replace comma with base type
        LexemeWord *commaWord = text->getTyped(curPos);
        memcpy(&temp, commaWord, sizeof(LexemeWord));
        memcpy(commaWord, type, sizeof(LexemeWord));

        // get resultType
        ResultType *resultType = GetDeclaringType();
        memcpy(commaWord, &temp, sizeof(LexemeWord));
        if (ErrorReported())
            return nullptr;
        LexemeWord *varname = text->getTyped(curPos++);
        if (!IsValidVarName(varname->lexeme))
        {
            ReportError(varname, "Expected variable identifier");
            return nullptr;
        }
        // create tnode
        TDeclaration *declaration = new TDeclaration(varname);
        declaration->parent = cur;
        declaration->arrayLength = nullptr;
        declaration->type = resultType;

        // parse declaration / initialization until , or ;
        if (text->getTyped(curPos)->code == 241) // =
        {
            // FOR CLASSES SKIP . AND :: HERE
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
                resultType->p_count++;
                if (declaration->arrayLength == nullptr)
                {
                    if (!ErrorReported())
                        ReportError(arrayLengthStart, "Array length expected");
                    return nullptr;
                }
                declaration->arrayLength->parent = nullptr;
                if (text->getTyped(curPos++)->code != 207) // ]
                {
                    if (!ErrorReported())
                        ReportError(arrayLengthStart, "Expected ']'");
                    return nullptr;
                }
            }
        }

        if (text->getTyped(curPos)->code == 243) // ;
        {
            curPos++;
            return root.children.takeFirst();
        }
        if (text->getTyped(curPos)->code != 242) // ,
        {
            ReportError(text->getTyped(curPos),
                        "Can not perform the operation on declaration. Expected ',' or ';'");
            return nullptr;
        }
        // handle ,
        bool temp1 = true, temp2 = false;
        TOperation *comma = GetTOperation(text->getTyped(curPos), temp1, temp2);
        comma->children.add(cur->children.takeLast());
        comma->children.getLast()->parent = comma;
        cur->children.add(comma);
        comma->parent = cur;
        cur = comma;
    }
    ReportError(0l, "Unknown error in declaration");
    return nullptr;
}

TOperation* SourceCodeParser::GetTypeCast(LexemeWord *word, bool &hasLeft, bool &expectedRight)
{
    if (hasLeft)
    {
        ReportError(word, "Unexpected type cast. Operation expected");
        return nullptr;
    }
    // get info about target typr
    // FOR CLASSES SKIP . AND :: HERE
    ResultType *targetType = GetDeclaringType();

    // check for terminal )
    LexemeWord *lastLexeme = text->getTyped(curPos++);
    if (lastLexeme->code != 205) // )
    {
        ReportError(lastLexeme, "Expected ')'");
        return nullptr;
    }
    hasLeft = false;
    expectedRight = true;
    return new TTypeCast(targetType, word);
}

TBranch *SourceCodeParser::HandleOperation(TBranch *cur, LexemeWord *word,
                                           bool &hasLeft, bool &expectedRight, bool stopOnComma)
{
    TOperation *operation;
    if (word->code == 204 && // (
        // STATIC MEMBERS PROBLEM HERE
        TypesManager::IsType(text->getTyped(curPos)->lexeme))
    {
        operation = GetTypeCast(word, hasLeft, expectedRight);
        if (operation != nullptr)
            word = operation->lexeme;
    }
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
        ((word->code == 243 || word->code == 205 || word->code == 207) // ; ) ]
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
        // change characteristics of the [
        cur->Priority = MINPRIORITY;
        cur->IsLeftAssociated = true;
        Heap::free_mem(cur->lexeme->lexeme);
        cur->lexeme->lexeme = copy_string("[]");
        return cur;
    }

    return nullptr;
}

TNode *SourceCodeParser::HandleExpression(bool stopOnComma)
{
    LexemeWord rootLexeme;
    rootLexeme.code = 200;
    TOperation root(&rootLexeme);
    root.Priority = 10000;
    TBranch *cur = &root;
    bool hasLeft = false, expectedRight = false;
    while (!ErrorReported() && cur != nullptr)
    {
        // FOR CLASSES GET MORE INFO ABOUT WORD HERE AND CHANGE
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
            if (TypesManager::IsType(*word)) // type declaration
                ReportError(word, "Unexpected type declaration. Maybe you miss ';'?");
            else
                cur = HandleTLeaf(cur, word, hasLeft, expectedRight); // variable
        }
        else if (hasLeft && word->code == 204) // (
            cur = HandleFunctionCall(cur, word, hasLeft, expectedRight); // function call
        else // operation
            cur = HandleOperation(cur, word, hasLeft, expectedRight, stopOnComma);
    }
    if (ErrorReported() || root.children.count() == 0)
        return nullptr;
    TNode *result = root.children.takeFirst();
    result->parent = nullptr;
    return result;
}

TNode* SourceCodeParser::ParseNextSentence(bool declarationAllowed)
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
             && TypesManager::IsType(*word))// type declaration
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
            ReportError(result->lexeme, "Sentence can not contain only constant or variable");
            return nullptr;
        }
        return result;
    }
    return nullptr;
}