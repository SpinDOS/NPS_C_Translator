#include "LexemeParser.h"
#include <climits>

bool TryParseAsCharConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseDouble(const char *s, unsigned long &length, double &result, NumType &numType, LexemeError *error);
bool TryParseOctal(const char *s, unsigned long &length, double &result, LexemeError *error);
bool TryParseHexadecimal(const char *s, unsigned long &length, double &result, LexemeError *error);
bool TryGetNum(const char *s, unsigned long &length, double &result, NumType &numType, LexemeError *error);
bool TryGetChar(const char *s, unsigned long &length, char &ch, LexemeError *error);

bool isDigit (char ch) { return ch >= '0' && ch <= '9'; }
bool is_var_char(char ch)
{ return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

bool TryParseAsConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    length = 0;
    error->message = "";
    switch (*s)
    {
        case '\'':
            return TryParseAsCharConstant(s, length, result, error);
        case '"':
            return TryParseAsStringConstant(s, length, result, error);
        default:
            break;
    }
    if (*s == '.' || isDigit(*s))
        return TryParseAsNumConstant(s, length, result, error);
    return false;
}

bool TryParseAsCharConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    char ch;
    length++;
    if (TryGetChar(s+1, length, ch, error) && *(s + length) == '\'')
    {
        length++;
        result->type_of_lexeme = CharConstant;
        result->description = Heap::get_mem(sizeof(char));
        *(static_cast<char *>(result->description)) = ch;
        return true;
    }
    
    const char *start = s;
    s += length;
    while (*s && *s != '\'' && *s != '\n')
        s++;
    if (*s == '\'')
        s++;
    length = s - start;
    error->error_start = start;
    error->invalid_lexeme = std::string(start, length);
    if (error->message.empty())
        error->message = "Invalid char constant";
    return false;
}

bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    return false;
    
}

bool TryParseAsNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    double d = 0;
    NumType numType;
    if (TryGetNum(s, length, d, numType, error))
    {
        result->type_of_lexeme = NumConstant;
        NumConstantDescription *description = static_cast<NumConstantDescription *>
        (Heap::get_mem(sizeof(NumConstantDescription)));
        description->num = d;
        if (numType == NumInt && d < 256)
            numType = NumChar;
        description->type = numType;
        result->description = description;
        return true;
    }
    
    const char *start = s;
    s += length;
    while (*s && (is_var_char(*s) || isDigit(*s)))
        length++;
    error->error_start = start;
    error->invalid_lexeme = string(s, length);
    if (error->message.empty())
        error->message = "Invalid numeric constant";
    return false;
    
}

bool TryGetNum(const char *s, unsigned long &length, double &result, NumType &numType, LexemeError *error)
{
    numType = NumInt;
    if (*s == '0' && isDigit(*(s + 1))) // octal
        return TryParseOctal(s+1, ++length, result, error);
    if (*s == '0' && *(s + 1) == 'x' && isDigit(*(s + 2))) // hexadecimal
        return TryParseHexadecimal(s+2, length += 2, result, error);
    // decimal
    return TryParseDouble(s, length, result, numType, error);
}

bool TryParseOctal(const char *s, unsigned long &length, double &result, LexemeError *error)
{
    result = 0;
    while (*s >= '0' && *s <= '7')
    {
        result = result * 8 + *s - '0';
        if (result > INT_MAX)
        {
            error->message = "Too large numeric constant";
            return false;
        }
        length++;
        s++;
    }
    return !(is_var_char(*s) || isDigit(*s));
}

int charToHexadecimal(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A';
    return -1;
}

bool TryParseHexadecimal(const char *s, unsigned long &length, double &result, LexemeError *error)
{
    result = 0;
    int curI = charToHexadecimal(*s);
    while (curI >= 0)
    {
        result = result * 16 + curI;
        if (result > INT_MAX)
        {
            error->message = "Too large numeric constant";
            return false;
        }
        curI = charToHexadecimal(*s);
        length++;
        s++;
    }
    return !(is_var_char(*s) || isDigit(*s));
}

bool TryParseDouble(const char *s, unsigned long &length, double &result, NumType &numType, LexemeError *error)
{
    result = 0;
    double floating_part = 0.1;
    int mantissa = 0;
    int mantissa_sign = 1;
    numType = NumInt;
    int curState = 0;
    while (true)
    {
        switch (curState)
        {
            case 0:
                if (isDigit(*s))
                    result = result * 10 + *s - '0';
                else if (*s == '.')
                    curState = 1;
                else if (*s == 'e')
                    curState = 3;
                else
                {
                    if (result > INT_MAX)
                    {
                        error->message = "Too large numeric constant";
                        return false;
                    }
                    return !is_var_char(*s);
                }
            break;
            case 1:
                numType = NumDouble;
                if (!isDigit(*s))
                    return false;
                else
                {
                    result = result + floating_part * (*s - '0');
                    floating_part /= 10;
                    curState = 2;
                }
            break;
            case 2:
                if (isDigit(*s))
                {
                    result = result + floating_part * (*s - '0');
                    double episilon = 0.00000000000001;
                    if (floating_part < episilon)
                    {
                        error->message = "Too small floating part in double constant";
                        return false;
                    }
                    floating_part /= 10;
                }
                else if (*s == 'e')
                    curState = 3;
                else
                    return !(*s == '.' || is_var_char(*s));
            break;
            case 3:
                numType = NumDouble;
                if (isDigit(*s))
                {
                    mantissa = mantissa * 10 + *s - '0';
                    curState = 5;
                }
                else if (*s == '-')
                {
                    mantissa_sign = -1;
                    curState = 4;
                }
                else if (*s == '+')
                {
                    curState = 4;
                }
                else
                    return false;
            break;
            case 4:
                if (!isDigit(*s))
                    return false;
                else
                {
                    mantissa = mantissa * 10 + *s - '0';
                    curState = 5;
                }
                break;
            case 5:
                if (*s == '.' || is_var_char(*s))
                    return false;
                else
                {
                    if (isDigit(*s))
                        mantissa = mantissa * 10 + *s - '0';
                    if (mantissa > 14)
                    {
                        error->message = "Too large mantissa in double constant";
                        return false;
                    }
                    while (mantissa != 0)
                    {
                        if (mantissa_sign > 0)
                            result *= 10;
                        else
                            result /= 10;
                        mantissa--;
                    }
                    return true;
                }
            break;
        }
        s++;
        length++;
    }
    
}

bool TryGetChar(const char *s, unsigned long &length, char &ch, LexemeError *error)
{
    if (!*s)
    {
        error->message = "Unexpected end of string";
        return false;
    }
    length++;
    if (*s != '\\')
    {
        ch = *s;
        return true;
    }
    s++;
    length++;
    switch (*s)
    {
        case 'r':
            ch = '\r';
            return true;
        case 'n':
            ch = '\n';
            return true;
        case 't':
            ch = '\t';
            return true;
        case 'v':
            ch = '\v';
            return true;
        case 'a':
            ch = '\a';
            return true;
        case '\"':
            ch = '\"';
            return true;
        case '\'':
            ch = '\'';
            return true;
        case '\\':
            ch = '\\';
            return true;
        case '\?':
            ch = '\?';
            return true;
        default:
            break;
    }
    length--;
    if (*s == 'x')
    {
        s++;
        length++;
        int code = charToHexadecimal(*s);
        if (code < 0)
        {
            error->message = "Invalid escape sequance";
            return false;
        }
        s++;
        length++;
        int secondCode = charToHexadecimal(*s);
        if (secondCode >= 0)
        {
            length++;
            code = code * 16 + secondCode;
        }
        ch = (char) code;
        return true;
    }
    if (isDigit(*s))
    {
        int code = 0;
        for (int i = 0; i < 3; i++)
        {
            if (!isDigit(*s))
                break;
            if (*s >= '8')
            {
                error->message = "Invalid octal code of char";
                return false;
            }
            code = code * 8 + *s - '0';
            length++;
            s++;
        }
        ch = (char) (code % 256);
        return true;
    }
    bool new_line = false;
    if (*s == '\r' && *(s+1) == '\n')
    {
        s += 2;
        length += 2;
        new_line = true;
    }
    else if (*s == '\n')
    {
        s++;
        length++;
        new_line = true;
    }
    if (new_line)
        return TryGetChar(s, length, ch, error);
    return false;
}





