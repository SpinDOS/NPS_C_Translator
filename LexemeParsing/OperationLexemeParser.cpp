#include "OperationToken.h"
#include "LexemeParser.h"

bool TryParseAsOperation(const char *s, unsigned long &length, LexemeInfo *result)
{
    char c = *s;
    OperationToken token;
    switch (c)
    {
        case '+':
            c = *(s+1);
            switch (c)
            {
                case '+':
                    token = TOKEN_INCREMENT;
                    length = 2;
                    break;
                default:
                    token = TOKEN_PLUS;
                    length = 1;
            }
            break;
        case '-':
            c = *(s+1);
            switch (c)
            {
                case '-':
                    token = TOKEN_DECREMENT;
                    length = 2;
                    break;
                case '>':
                    token = TOKEN_STRUCTURE_DEREFERENCE;
                    length = 2;
                    break;
                default:
                    token = TOKEN_MINUS;
                    length = 1;
            }
            break;
        case '=':
            c = *(s+1);
            switch (c)
            {
                case '=':
                    token = TOKEN_EQUALS;
                    length = 2;
                    break;
                default:
                    token = TOKEN_ASSIGN;
                    length = 1;
            }
            break;
        case '>':
            c = *(s+1);
            switch (c)
            {
                case '=':
                    token = TOKEN_MORE_EQ;
                    length = 2;
                    break;
                case '>':
                    token = TOKEN_BITWISE_SHIFT_RIGHT;
                    length = 2;
                    break;
                default:
                    token = TOKEN_MORE;
                    length = 1;
            }
            break;
        case '<':
            c = *(s+1);
            switch (c)
            {
                case '=':
                    token = TOKEN_LESS_EQ;
                    length = 2;
                    break;
                case '<':
                    token = TOKEN_BITWISE_SHIFT_LEFT;
                    length = 2;
                    break;
                default:
                    token = TOKEN_LESS;
                    length = 1;
            }
            break;
        case '!':
            c = *(s+1);
            switch (c)
            {
                case '=':
                    token = TOKEN_NOT_EQUALS;
                    length = 2;
                    break;
                default:
                    token = TOKEN_NEGATION;
                    length = 1;
            }
            break;
        case '|':
            c = *(s+1);
            switch (c)
            {
                case '|':
                    token = TOKEN_LOGIC_OR;
                    length = 2;
                    break;
                default:
                    token = TOKEN_BITWISE_OR;
                    length = 1;
            }
            break;
        case '&':
            c = *(s+1);
            switch (c)
            {
                case '&':
                    token = TOKEN_LOGIC_AND;
                    length = 2;
                    break;
                default:
                    token = TOKEN_BITWISE_AND;
                    length = 1;
            }
        case '/':
            token = TOKEN_DIVISION;
            length = 1;
            break;
        case '%':
            token = TOKEN_MODULO;
            length = 1;
            break;
        case '~':
            token = TOKEN_BITWISE_NOT;
            length = 1;
            break;
        case '^':
            token = TOKEN_BITWISE_XOR;
            length = 1;
            break;
        case '.':
            token = TOKEN_STRUCTURE_REFERENCE;
            length = 1;
            break;
        case ',':
            token = TOKEN_COMMA;
            length = 1;
            break;
        case '?':
            token = TOKEN_TERN_COND_IF;
            length = 1;
            break;
        case ':':
            token = TOKEN_TERN_COND_ELSE;
            length = 1;
            break;
        case ';':
            token = TOKEN_SEMICOLON;
            length = 1;
            break;
        case '{':
            token = TOKEN_START_CURLY_BRACKET;
            length = 1;
            break;
        case '}':
            token = TOKEN_END_CURLY_BRACKET;
            length = 1;
            break;
        case '[':
            token = TOKEN_START_SQUARE_BRACKET;
            length = 1;
            break;
        case ']':
            token = TOKEN_END_SQUARE_BRACKET;
            length = 1;
            break;
        case '(':
            token = TOKEN_START_BRACKET;
            length = 1;
            break;
        case ')':
            token = TOKEN_END_BRACKET;
            length = 1;
            break;
        case '*':
            token = TOKEN_MULTIPL;
            length = 1;
            break;
        default:
            length = 0;
            return false;
    }
    result->type_of_lexeme = Operation;
    result->description = Heap::get_mem(sizeof(OperationToken));
    *static_cast<OperationToken *>(result->description) = token;
    return true;
}