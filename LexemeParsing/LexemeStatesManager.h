//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMESTATESMANAGER_H
#define NPS_C_TRANSLATOR_LEXEMESTATESMANAGER_H

#include "../collection_containers/hash.h"
#include "../utils/string_utils.h"

class LexemeCase
{
public:
    virtual ~LexemeCase(){};
    virtual bool IsMyChar(char ch) = 0;
    virtual int GetNextState() = 0;
};

class CharRange
{
    char start, end;
public:
    CharRange(char _start, char _end) {start = _start; end = _end; }
    bool IsMyChar(char ch) { return ch >= start && ch <= end; }
};

class RangeCase : LexemeCase
{
    CharRange *range;
    int newState;
public:
    RangeCase(CharRange *_range, int _newState)
                { range = _range; newState = _newState; }
    bool IsMyChar(char ch) override {return range->IsMyChar(ch);}
    int GetNextState() override {return newState;}
};

class StringCase : LexemeCase
{
    char *str;
    int newState;
public:
    StringCase(const char *start, unsigned long length, int _newState)
    {str = copy_string(start, length); newState = _newState;}
    ~StringCase() override {Heap::free_mem(str);}
    bool IsMyChar(char ch) override
    {
        const char *cur = str;
        while (*cur && *cur != ch)
            cur++;
        return *cur == ch;
    }
    int GetNextState() override {return newState;}
};

class LexemeStatesManager : private Hash
{
public:
    LexemeStatesManager() : Hash(10, 10, 10, 0 , 0) { }
    ~LexemeStatesManager();
    int key1(const char *key_word) override
        { return (*reinterpret_cast<const int*>(key_word)/100) % 10; };
    int key2(const char *key_word) override
        { return (*reinterpret_cast<const int*>(key_word)/10) % 10; };
    int key3(const char *key_word) override
        { return *reinterpret_cast<const int*>(key_word) % 10; };
    
    int FindNextState(int currentState, char currentChar);
    void Add(LexemeCase *lexemeCase, int state);
};

#endif //NPS_C_TRANSLATOR_LEXEMESTATESMANAGER_H
