#include "LexemeParser.h"

bool TryParseAsCharConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseDouble(const char *s, unsigned long &length, double &result, NumType &numType);
bool TryParseOctal(const char *s, unsigned long &length, double &result);
bool TryParseHexadecimal(const char *s, unsigned long &length, double &result);
bool TryGetNum(const char *s, unsigned long &length, double &result, NumType &numType);

bool isDigit (char ch) { return ch >= '0' && ch <= '9'; }
bool is_var_char(char ch)
{ return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

bool TryParseAsConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    length = 0;
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
    
   
    {
        const char *start = s++;
        while (*s && *s != '\'' && *s != '\n')
            s++;
        length = s - start;
        error->error_start = start;
        error->invalid_lexeme = std::string(start, length);
        error->message = "Invalid char constant";
        return false;
    }
}

bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    return false;
    
}

bool TryParseAsNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    double d = 0;
    NumType numType;
    if (TryGetNum(s, length, d, numType))
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
    error->message = "Invalid numeric constant";
    return false;
    
}

bool TryGetNum(const char *s, unsigned long &length, double &result, NumType &numType)
{
    numType = NumInt;
    if (*s == '0' && isDigit(*(s + 1))) // octal
        return TryParseOctal(s+1, ++length, result);
    if (*s == '0' && *(s + 1) == 'x') // hexadecimal
        return TryParseHexadecimal(s+1, ++length, result);
    // decimal
    return TryParseDouble(s, length, result, numType);
}

bool TryParseOctal(const char *s, unsigned long &length, double &result)
{
    result = 0;
    while (*s >= '0' && *s <= '7')
    {
        result = result * 8 + *s - '0';
        length++;
        s++;
    }
    return !(is_var_char(*s) || isDigit(*s));
}

bool TryParseAsDouble(const char *s, unsigned long &length, double &result, NumType &numType)
{
    result = 0;
    double floating_part = 0.1;
    int mantissa = 0;
    int mantissa_sign = 0;
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
                    return !is_var_char(*s);
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
                    floating_part /= 10;
                }
                else if (*s == 'e')
                    curState = 3;
                else
                    return !(*s == '.' || is_var_char(*s));
            break;
            case 3:
                numType = NumDouble;
                if (*s == '+')
                    mantissa_sign = 1;
                else if (*s == '-')
                    mantissa_sign = -1;
                else
                    return false;
            curState = 4;
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
                if (isDigit(*s))
                    mantissa = mantissa * 10 + *s - '0';
                else if (*s == '.' || is_var_char(*s))
                    return false;
                else
                {
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





