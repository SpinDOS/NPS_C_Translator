#include "LexemeParser.h"
#include <climits>

bool TryParseAsCharConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsStringConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseAsNumConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error);
bool TryParseDouble(const char *s, unsigned long &length, double &result, NumType &numType, LexemeError *error);
bool TryParseOctal(const char *s, unsigned long &length, double &result, LexemeError *error);
bool TryParseHexadecimal(const char *s, unsigned long &length, double &result, LexemeError *error);
bool TryGetNum(const char *s, unsigned long &length, double &result, NumType &numType, LexemeError *error);

bool isDigit (char ch) { return ch >= '0' && ch <= '9'; }
bool is_var_char(char ch)
{ return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

bool TryParseAsConstant(const char *s, unsigned long &length, LexemeInfo *result, LexemeError *error)
{
    length = 0;
    error->message = nullptr;
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
    if (error->message != nullptr)
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
                    if (floating_part < 0.00000000000001)
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





