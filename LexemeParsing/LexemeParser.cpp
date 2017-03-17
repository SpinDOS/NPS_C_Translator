//
// Created by Alexander on 16-Mar-17.
//

#include "LexemeParser.h"
#include <fstream>
#include <cstring>

using namespace std;

void AddState(string line, LexemeStatesManager &statesManager, CharsRange **charRanges);
void SplitStringToTokens(string str, int &oldState, string &pattern, int &newState);

LexemeParser::LexemeParser(const char *filename)
{
    memset(charRanges, 0, CHARRANGESCOUNT * sizeof(CharsRange*));
    string str;
    ifstream file;
    file.open(filename);
    while (!file.eof())
    {
        getline(file, str, '\n');
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
    file.close();
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

