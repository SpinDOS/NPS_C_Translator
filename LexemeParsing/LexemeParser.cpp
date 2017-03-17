//
// Created by Alexander on 16-Mar-17.
//

#include "LexemeParser.h"
#include <fstream>
#include <cstring>

using namespace std;

void AddState(string line, LexemeStatesManager *statesManager, CharsRange **charRanges);
void SplitStringToTokens(string str, int &oldState, string &pattern, int &newState);
void GetGroupsAndSimpleChars(const char *str, const char **groupsChars, const char **simpleChars);

LexemeParser::LexemeParser(const char *filename)
{
    memset(charRanges, 0, CHARRANGESCOUNT * sizeof(CharsRange*));
    statesManager = new LexemeStatesManager;
    string str;
    ifstream file;
    file.open(filename);
    while (!file.eof())
    {
        getline(file, str, '\n');
        if (str.length() <= 1)
            continue;
        if (str[0] == '#')
            charRanges[str[1] - 'A'] = new CharsRange(str[3], str[5]);
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
    delete statesManager;
}

void AddState(string line, LexemeStatesManager *statesManager, CharsRange **charRanges)
{
    int oldState, newState;
    string pattern;
    SplitStringToTokens(line, oldState, pattern, newState);
    
    const char *groupsChars, *simpleChars;
    GetGroupsAndSimpleChars(pattern.c_str(), &groupsChars, &simpleChars);
    
    if (simpleChars != nullptr)
        statesManager->Add(oldState, new StringCase(simpleChars, newState));
    
    if (groupsChars == nullptr)
        return;
    
    while (*groupsChars && *groupsChars != '#')
    {
        if (*groupsChars == 'S')
            statesManager->Add(oldState,
                               new SpecialGroupCase(newState));
        else
        {
            CharsRange *charsRange = charRanges[*groupsChars - 'A'];
            statesManager->Add(oldState,
                               new RangeCase(charsRange, newState));
        }
        groupsChars++;
    }
    
}

void SplitStringToTokens(string str, int &oldState, string &pattern, int &newState)
{
    oldState = stoi(str);
    str = str.substr(str.find(' ') + 1);
    int spacePos = str.find(' ');
    pattern = str.substr(0, spacePos).c_str();
    newState = stoi(str.substr(spacePos + 1));
}

void GetGroupsAndSimpleChars(const char *str, const char **groupsChars, const char **simpleChars)
{
    *groupsChars = nullptr;
    *simpleChars = nullptr;
    if (*str != '#')
    {
        *simpleChars = str;
        return;
    }
    *groupsChars = str + 1;
    char *end = strchr(*groupsChars, '#');
    if (end != nullptr)
        *simpleChars = end + 1;
}


