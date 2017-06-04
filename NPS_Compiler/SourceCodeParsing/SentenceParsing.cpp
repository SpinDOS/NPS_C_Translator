//
// Created by Alexander on 18-Apr-17.
//

#include "SourceCodeParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

bool SourceCodeParser::IsValidVarName(LexemeWord *var)
{ return 400 <= var->code && var->code < 600 && !TypesManager::IsType(var->lexeme); }

bool SourceCodeParser::ThrowIfEndOfFile()
{
    if (!IsEnd())
        return false;
    if (!ErrorReported())
        ReportError(text->getTyped(curPos - 1), "Unexpected end of file");
    return true;
}

bool SourceCodeParser::IsType()
{
    int temp = curPos;
    ResultType *result = TryGetResultType();
    curPos = temp;
    return result != nullptr;
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
            if (nextType->code < 400 || nextType->code >= 600)
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
        while (!IsEnd() && text->getTyped(curPos)->code == 218) // *
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
            if (!IsEnd() && text->getTyped(curPos)->code == 205) // )
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

bool SourceCodeParser::TryGetDeclaration(TSimpleLinkedList<TNode> *list)
{
    if (IsEnd())
        return false;
    ResultType *type = TryGetResultType();
    if (type == nullptr)
        return false;
    
    if (ThrowIfEndOfFile())
        return false;
    LexemeWord *name = text->getTyped(curPos++);
    if (!IsValidVarName(name))
    {
        ReportError(name, "Invalid variable name");
        return false;
    }
    
    if (ThrowIfEndOfFile())
        return false;
    LexemeWord *next = text->getTyped(curPos++);
    
    // function declaration
    if (next->code == 204) // (
    {
        TFunctionDefinition *functionDefinition = GetFunctionDefinition(type, name);
        if (ErrorReported())
            return false;
        list->add(functionDefinition);
        return true;
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
    
        if (next->code != 206) // [
        {
            TVariableDeclaration *result = new TVariableDeclaration(name);
            result->declaring_type = type;
            list->add(result);
            if (next->code == 241) // =
            {
                curPos -= 2; // revert to name
                list->add(HandleExpression(true));
                if (ErrorReported())
                    return false;
                
                if (ThrowIfEndOfFile())
                    return false;
                next = text->getTyped(curPos++);
            }
        }
        else
        { // array
            TArrayDeclaration *result = new TArrayDeclaration(name);
            result->declaring_type = type;
            if (ThrowIfEndOfFile())
                return false;
            result->array_length = HandleExpression(false);
            if (result->array_length == nullptr)
            {
                if (!ErrorReported())
                    ReportError(text->getTyped(curPos - 1), "Expected array length");
                return false;
            }
            if (IsEnd() || text->getTyped(curPos++)->code != 207) // ]
            {
                ReportError(text->getTyped(curPos - 1), "Expected ']'");
                return false;
            }
            list->add(result);
    
            if (ThrowIfEndOfFile())
                return false;
            next = text->getTyped(curPos++);
        }
        
        if (next->code == 243) // ;
            break;
        else if (next->code != 242) // ,
        {
            ReportError(next, "Expected ',' or ';' in the declaration list");
            return false;
        }
        
        if (ThrowIfEndOfFile())
            return false;
        if (text->getTyped(curPos)->code != 218) // *
        {
            type = underlying;
            used = true;
        }
        else
        {
            type = underlying->clone();
            do
                type->p_count++;
            while (!IsEnd() && text->getTyped(++curPos)->code == 218); // *
        }
    
        if (ThrowIfEndOfFile())
            return false;
        name = text->getTyped(curPos++);
        if (!IsValidVarName(name))
        {
            ReportError(name, "Invalid variable name");
            return false;
        }
        if (ThrowIfEndOfFile())
            return false;
        next = text->getTyped(curPos++);
    }
    if (!used)
        delete underlying;
    return true;
}

TFunctionDefinition* SourceCodeParser::GetFunctionDefinition(ResultType *readBeforeReturnType, LexemeWord *name)
{ // expected that 'ReturnType Name (' is read before
    TFunctionDefinition *result = new TFunctionDefinition(name);
    result->returnValue = readBeforeReturnType;
    
    if (ThrowIfEndOfFile())
        return nullptr;
    if (text->getTyped(curPos)->code != 205) // )
    {
        while (true)
        {
            FunctionParameterInfo *param = new FunctionParameterInfo;
            result->parameters.add(param);
            
            if (ThrowIfEndOfFile())
                return false;
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
            
            if (ThrowIfEndOfFile())
                return nullptr;
            param->name = text->getTyped(curPos++);
            if (!IsValidVarName(param->name))
            {
                ReportError(param->name, "Invalid parameter name");
                return nullptr;
            }
            for (int i = 0; i < result->parameters.count() - 1; i++)
                if (strcmp(result->parameters.get(i)->name->lexeme, param->name) == 0)
                {
                    ReportError(name, "Paramater with the same name already exists");
                    return nullptr;
                }
            
            if (ThrowIfEndOfFile())
                return nullptr;
            int next_code = text->getTyped(curPos++)->code;
            if (next_code == 241) // =
            {
                if (ThrowIfEndOfFile())
                    return nullptr;
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
                
                if (ThrowIfEndOfFile())
                    return nullptr;
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
    
    if (IsEnd() || text->getTyped(curPos)->code != 200) // {
    {
        ReportError(text->getTyped(curPos), "Function implementation expected");
        return nullptr;
    }
    result->implementation = ParseList();
    if (ErrorReported())
        return nullptr;
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

TBranch *SourceCodeParser::HandleFunctionCall(TBranch *cur, LexemeWord *word, bool &hasLeft, bool &expectedRight)
{ // expected that word is '(', curpos is on next position
    // on exit, curpos is on next symbol after ')'
    
    // handle tnode
    TFunction *function = new TFunction(word);
    function->parent = cur;
    function->function = cur->children.takeLast();
    function->function->parent = function;
    cur->children.add(function);

    // parse arguments
    if (ThrowIfEndOfFile())
        return nullptr;
    if (text->getTyped(curPos)->code != 205) // )
        while (true) {
            TNode *arg = HandleExpression(true);
            if (ErrorReported())
                return nullptr;
            if (arg == nullptr) {
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
        ReportError(text->getTyped(curPos - 1), "Expected ')'");
        return nullptr;
    }
    function->lexeme->code = 205;
    Heap::free_mem(function->lexeme->lexeme);
    function->lexeme->lexeme = copy_string("()");
    hasLeft = true;
    expectedRight = false;
    return cur;
}

TOperation* SourceCodeParser::GetTypeCast(LexemeWord *word, bool &hasLeft, bool &expectedRight)
{// expected that curpos is on type name
    // on exit, curpos is after ')'
    if (hasLeft)
    {
        ReportError(word, "Unexpected type cast. Operation expected");
        return nullptr;
    }
    if (ThrowIfEndOfFile())
        return nullptr;
    ResultType *targetType = TryGetResultType();
    if (ThrowIfEndOfFile())
        return nullptr;

    // check for terminal )
    if (text->getTyped(curPos++)->code != 205) // )
    {
        ReportError(text->getTyped(curPos - 1), "Expected ')'");
        return nullptr;
    }
    word->code = 205; // )
    Heap::free_mem(word->lexeme);
    word->lexeme = copy_string("(type)");
    hasLeft = false;
    expectedRight = true;
    return new TTypeCast(targetType, word);
}

TBranch *SourceCodeParser::HandleOperation(TBranch *cur, LexemeWord *word,
                                           bool &hasLeft, bool &expectedRight, bool stopOnComma)
{ // expected that word is operation, curpos is on next symbol
    // on exit, curpos is on next symbol after operation
    if (ThrowIfEndOfFile())
        return nullptr;
    TOperation *operation;
    if (word->code == 204 && IsType())
        operation = GetTypeCast(word, hasLeft, expectedRight);
    else if (ErrorReported())
        return nullptr;
    else
        operation = GetTOperation(word, hasLeft, expectedRight);

    if (ErrorReported() || ThrowIfEndOfFile())
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
        (word->code == 243 || word->code == 207 || // ; ]
         (word->code == 205 && operation->tNodeType != TNodeTypeCast) || // (
         (stopOnComma && word->code == 242))) // ,
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

TNode *SourceCodeParser::HandleExpression(bool stopOnComma)
{ // expected that curpos is on first expression symbol
    // on exit, curpos is on ';' or smth like it
    
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
            // new, delete keywords here
            ReportError(word, "Unexpected keyword. Maybe you miss ';'?");
        }
        else if (400 <= word->code && word->code < 600)// varname
        {
            if (IsType()) // type declaration
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

bool SourceCodeParser::ParseNextSentence(TSimpleLinkedList<TNode> *list, bool declarationAllowed)
{ // expected that curpos is on first sentence symbol
    // on exit, curpos is after ';'
    if (ThrowIfEndOfFile())
        return false;
    LexemeWord *word = text->getTyped(curPos);
    TNode *result = nullptr;
    if (300 <= word->code && word->code < 400)// keyword
    {
        switch (word->code)
        {
            case 308: // do
                result = HandleKeywordDoWhile();
                break;
            case 330: // while
                result = HandleKeywordWhile();
                break;
            case 312: // for
                result = HandleKeywordFor();
                break;
            case 313: // if
                result = HandleKeywordIf();
                break;
            case 326: // switch
                result = HandleKeywordSwitch();
                break;
            case 301: // break
            case 305: // continue
                result = HandleKeywordBreakContinue();
                break;
            case 323: // return
                result = HandleKeywordReturn();
                break;
            case 318: // new
                //return HandleExpression(false);
                ReportError(word, "This keyword is not implemented yet");
                return false;
            default:
                ReportError(word, "This keyword can not be used here");
                return false;
        }
    }
    else if (400 <= word->code && word->code < 600
             && IsType())// type declaration
    {
        if (declarationAllowed)
            return TryGetDeclaration(list);
        ReportError(word, "Type declaration is not allowed here");
        return false;
    }
    else if (ErrorReported())
        return false;
    else if (word->code == 200) // {
        result = ParseList();
    else
    {
        result = HandleExpression(false);
        if (ErrorReported())
            return false;
        
        if (ThrowIfEndOfFile())
            return false;

        // validate terminating ;
        word = text->getTyped(curPos++);
        if (word->code != 243) // ;
        {
            ReportError(word, "Expected ';' ");
            return false;
        }
        if (result == nullptr)
            return true;

        // check for a;
        if (result->tNodeType == TNodeTypeConstant ||
            result->tNodeType == TNodeTypeVariable)
        {
            ReportError(result->lexeme, "Sentence can not contain only constant or variable");
            return nullptr;
        }
    }
    list->add(result);
    return true;
}
