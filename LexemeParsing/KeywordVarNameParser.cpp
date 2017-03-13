//
// Created by murchan on 3/12/17.
//
#include "LexemeParser.h"
#include "../utils/string_utils.h"
#include "Keywords.h"

const char *keywords[15][10] = {
        //Loop operators
        {"for", "while", "do", "break", "continue"},
        //RETURN
        {"return"},
        //IF-ELSE
        {"if", "else"},
        //SWITCH-CASE-DEFAULT
        {"switch", "case", "default"},
        //True-false
        {"true", "false"},
        //Access modifiers
        {"private", "public", "protected"},
        //Structure types
        {"class", "struct", "enum", "namespace"},
        //Variable types
        {"void", "char", "bool", "double", "int"},
        //CONST-STATIC
        {"const", "static"},
        //VIRTUAL
        {"virtual"},
        //Other functions
        {"input", "output", "sin", "cos", "max", "min", "new"}
};

bool TryParseAsKeywordOrVariable(const char *s, unsigned long &length, LexemeInfo *result)
{
    const char *start = s;
    if (*s >= '0' && *s <= '9')
        return false;
    for (; *s && (*s >= '0' && *s <= '9') ||
           *s == '_' || (*s <= 'z' &&
            *s >= 'a') || (*s <= 'Z' &&
            *s >= 'A'); s++)
    {}
    length = s - start;
    if (length > 0)
    {
        char *currentWord = copy_string(start, length);
        if (getKeywordCode(currentWord) != -1){
            result -> type_of_lexeme = Keyword;
            //не уверена, просто не знаю, как еще можно запихнуть int, когда нужен указатель
            result -> description = (int *) getKeywordCode(currentWord);
            return true;
        }
        else{
            result -> type_of_lexeme = VariableName;
            result -> description = currentWord;
            return true;
        }
    }
    else
        return false;
}

int getKeywordCode (const char *keyword){
        for (int i = 0; i < 11; i++)
            for (int j = 0; j < 7 && getKeyword(i*10+j) != nullptr; j++)
                if (compare_strings(keyword, keywords[i][j]))
                    return i*10+j;
        return -1;
}

const char *getKeyword(int index){
    int groupNumber = index/10;
    int keywordNumber = index%10;
    bool keywordExists = false;
    switch(groupNumber)
    {
        case 0:
            if (keywordNumber < 5)
                keywordExists = true;
            break;
        case 1:
            if (keywordNumber < 1)
                keywordExists = true;
            break;
        case 2:
            if (keywordNumber < 2)
                keywordExists = true;
            break;
        case 3:
            if (keywordNumber < 3)
                keywordExists = true;
            break;
        case 4:
            if (keywordNumber < 2)
                keywordExists = true;
            break;
        case 5:
            if (keywordNumber < 3)
                keywordExists = true;
            break;
        case 6:
            if (keywordNumber < 4)
                keywordExists = true;
            break;
        case 7:
            if (keywordNumber < 5)
                keywordExists = true;
            break;
        case 8:
            if (keywordNumber < 2)
                keywordExists = true;
            break;
        case 9:
            if (keywordNumber < 1)
                keywordExists = true;
            break;
        case 10:
            if (keywordNumber < 7)
                keywordExists = true;
            break;
        default:
            break;
    }
    if (keywordExists)
            return keywords[groupNumber][keywordNumber];
        else
            return nullptr;
}