//
// Created by Alexander on 16-Mar-17.
//

#include "LexemeParser.h"
#include <sstream>
#include <cstring>

using namespace std;

void AddState(string line, LexemeStatesManager &statesManager, CharsRange **charRanges);
void SplitStringToTokens(string str, int &oldState, string &pattern, int &newState);
char get_char(const char **str);

LexemeParser::LexemeParser(const char *instructions)
{
    memset(charRanges, 0, CHARRANGESCOUNT * sizeof(CharsRange*));
    charRanges['a' - 'A'] = new CharsRange(0, 127);
    istringstream reader(instructions);
    string str;
    while (std::getline(reader, str))
    {
        if (str.length() <= 1)
            continue;
        if (str[0] == '#')
        {
            CharsRange *charsRange;
            if (str[4] == ' ') // #a a b
                charsRange = new CharsRange(str[3], str[5]);
            else // #a #10
                charsRange = new CharsRange(static_cast<char>
                                            (stoi(str.substr(4))));
            charRanges[str[1] - 'A'] = charsRange;
        }
        else if (isdigit(str[0]))
            AddState(str, statesManager, charRanges);
    }
}

LexemeParser::~LexemeParser()
{
    for (int i = 0; i < CHARRANGESCOUNT; i++)
        if (charRanges[i] != nullptr)
            delete charRanges[i];
}

void AddState(string line, LexemeStatesManager &statesManager, CharsRange **charRanges)
{
    int oldState, newState;
    string pattern;
    SplitStringToTokens(line, oldState, pattern, newState);
    
    if (pattern[0] == '#' && pattern[1] != ' ')
    {
        for (int i = 1; i < pattern.length(); i++)
        {
            CharsRange *charsRange = charRanges[pattern[i] - 'A'];
            statesManager.Add(oldState, new RangeCase(charsRange, newState));
        }
    }
    else
        statesManager.Add(oldState, new StringCase(pattern.c_str(), newState));
}

void SplitStringToTokens(string str, int &oldState, string &pattern, int &newState)
{
    oldState = stoi(str);
    str = str.substr(str.find(' ') + 1);
    int spacePos = str.find(' ');
    pattern = str.substr(0, spacePos).c_str();
    newState = stoi(str.substr(spacePos + 1));
}

bool LexemeParser::ParseToLexemes(const char *fileContent, TypeList<LexemeWord> &words, LexemeError &error)
{
    int curState = 0;
    const char *word_start = nullptr;
    while (*fileContent)
    {
        int newState = statesManager.FindNextState(curState, *fileContent);
        if (1 <= newState && newState <= 99)
        {
            error.errorCode = newState;
            error.errorStart = fileContent;
            error.lexemeCode = curState;
            error.lexemeStart = word_start;
            return false;
        }
        else if (newState != 0 && curState == 0)
        {
            word_start = fileContent;
        }
        else if (newState == 0 && 0 < curState && curState < 600)
        {
            LexemeWord word;
            word.start = word_start;
            word.length = fileContent - word_start;
            word.code = curState;
            words.addTyped(word);
            word_start = nullptr;
            --fileContent;
        }
        curState = newState;
        fileContent++;
    }
    
    if (word_start == nullptr || curState >= 600)
        return true;
    
    if (statesManager.FindNextState(curState, *fileContent) == 0)
    {
        LexemeWord word;
        word.start = word_start;
        word.length = fileContent - word_start;
        word.code = curState;
        words.addTyped(word);
        return true;
    }
    error.errorCode = 1;
    error.errorStart = fileContent;
    error.lexemeCode = curState;
    error.lexemeStart = word_start;
    return false;
}

char parse_char_constant(LexemeWord &word, LexemeError &error)
{
    const char *str = word.start + 1;
    char result = get_char(&str);
    if (*str == '\'')
        return result;
    error.errorCode = 21;
    error.errorStart = str;
    error.lexemeCode = word.code;
    error.lexemeStart = word.start;
    return 0;
}

char* parse_string_constant(LexemeWord &word)
{
    char *result = static_cast<char*>(Heap::get_mem(word.length - 2));
    int index = 0;
    const char *cur = word.start + 1;
    while (*cur != '"')
        result[index++] = get_char(&cur);
    result[index] = 0;
    return result;
}

double parse_num_constant(LexemeWord &word, NumConstantType &type, LexemeError &error)
{
    string str(word.start, word.length);
    double result;
    type = IntConstant;
    bool err = false;
    if (120 <= word.code && word.code < 130) // decimal
    {
        if (word.code != 120) // double
        {
            type = DoubleConstant;
            if (word.length > 16)
                err = true;
            else
            {
                int epos = str.find('e');
                if (epos > 0 && (epos + 10 < word.length ||
                        stoi(str.substr(epos + 1)) > 14))
                    err = true;
            }
        }
        else if (word.length > 10) // int overflow
            err = true;
        if (!err)
            result = stod(str); // int
    }
    if (130 <= word.code && word.code < 140) // hexadecimal
    {
        if (word.length > 9)
            err = true;
        else
            result = stoi(str, nullptr, 16);
    }
    if (140 <= word.code && word.code < 150) // octal
    {
        if (word.length > 11)
            err = true;
        else
            result = stoi(str, nullptr, 8);
    }
    if (err)
    {
        error.errorCode = 2;
        error.errorStart = word.start + word.length - 1;
        error.lexemeCode = word.code;
        error.lexemeStart = word.start;
        return -1;
    }
    if (type == IntConstant && result < 128)
        type = CharConstant;
    return result;
}

char get_char(const char **str) // move str pointer
{
    char ch;
    do // skip \n's
    {
        ch = *((*str)++);
        if (ch != '\\')
            return ch;
        ch = *((*str)++);
    }
    while (ch == '\n');
    
    if (isdigit(ch)) // if the char is escaped number(\95)
    {
        ch -= '0';
        char temp = *((*str)++);
        if (!isdigit(temp))
        {
            (*str)--;
            return ch;
        }
        ch = ch * 10 + temp - '0';
        temp = *((*str)++);
        if (!isdigit(temp) ||
                ch * 10 + temp - '0' >= 128)
        {
            (*str)--;
            return ch;
        }
        return ch * 10 + temp - '0';
    }
    switch (ch) // escaped sequence or ignore '\'
    {
        case 'r':
            return '\r';
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case 'a':
            return '\a';
        default:
            return ch;
    }
}