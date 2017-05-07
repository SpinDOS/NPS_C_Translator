//
// Created by Alexander on 16-Mar-17.
//

#include "../ErrorReporter/ErrorReporter.h"
#include "LexemeParser.h"
#include <sstream>
#include <cstring>

using namespace std;

void AddState(string line, LexemeStatesManager &statesManager, CharsRange **charRanges);
void SplitStringToTokens(string str, int &oldState, string &pattern, int &newState);
char get_char(const char **str);
const char* get_error_message(int code);


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

void LexemeParser::ParseToLexemes(const char *fileContent, TypeList<LexemeWord> &words)
{
    int curState = 0;
    const char *start = fileContent;
    const char *word_start = nullptr;
    do
    {
        int newState = statesManager.FindNextState(curState, *fileContent);
        if (1 <= newState && newState <= 99)
        {
            ReportError(word_start - start, get_error_message(newState));
            return;
        }
        else if (newState == 0 && 0 < curState && curState < 600)
        {
            LexemeWord word;
            word.lexeme = copy_string (word_start, fileContent - word_start);
            word.positionInTheText = word_start - start;
            word.code = curState;
            words.addTyped(word);
            word_start = nullptr;
            fileContent--;
        }
        else if (newState != 0 && curState == 0)
            word_start = fileContent;
        curState = newState;
    }
    while (*fileContent++);
    
    if (curState != 0)
        ReportError(word_start - start, "Unexpected end of file");
}

char parse_char_constant(LexemeWord &word)
{
    const char *str = word.lexeme + 1;
    char result = get_char(&str);
    if (*str == '\'')
        return result;
    ReportError(word.positionInTheText, "Multicharacter constant");
    return 0;
}

char* parse_string_constant(LexemeWord &word)
{
    char *result = Heap::get_mem(strlen(word) - 1);
    int index = 0;
    const char *cur = word.lexeme + 1;
    while (*cur != '"')
        result[index++] = get_char(&cur);
    result[index] = 0;
    return result;
}

double parse_num_constant(LexemeWord &word, char **type_buffer)
{
    string str(word.lexeme);
    double result;
    bool isDouble = false;
    bool err = false;
    if (120 <= word.code && word.code < 130) // decimal
    {
        if (word.code != 120) // double
        {
            isDouble = true;
            if (str.length() > 16)
                err = true;
            else
            {
                int epos = str.find('e');
                if (epos > 0 && (epos + 10 < str.length() ||
                        stoi(str.substr(epos + 1)) > 14))
                    err = true;
            }
        }
        else if (str.length() > 10) // int overflow
            err = true;
        if (!err)
            result = stod(str); // int
    }
    if (130 <= word.code && word.code < 140) // hexadecimal
    {
        if (str.length() > 9)
            err = true;
        else
            result = stoi(str, nullptr, 16);
    }
    if (140 <= word.code && word.code < 150) // octal
    {
        if (str.length() > 11)
            err = true;
        else
            result = stoi(str, nullptr, 8);
    }
    if (err)
    {
        ReportError(word.positionInTheText, "Too large number");
        return -1;
    }
    if (isDouble)
        *type_buffer = copy_string("double");
    else
        *type_buffer = copy_string("int");
    return result;
}

bool parse_bool_constant(LexemeWord &word)
{return word.code == 150;} // 150 - true

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

const char* get_error_message(int code)
{
    switch (code)
    {
        case 10:
            return "Unexpected newline inside the string or char constant";
        case 20:
            return "Empty char constant";
        case 22:
            return "Invalid escaped sequence";
        case 23:
            return "Invalid escaped character's code";
        case 30:
            return "Numeric constant ends with invalid character";
        case 31:
            return "Mantissa character e is followed by invalid char";
        case 32:
            return "Mantissa sign character is followed by invalid char";
        case 33:
            return "Octal number contains invalid characters";
        case 34:
            return "Invalid hexadecimal number after 0x";
        case 40:
            return "Unexpected end of file (comment is not closed)";
        default:
            return "Unknown error";
    }
}