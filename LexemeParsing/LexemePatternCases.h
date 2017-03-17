//
// Created by Alexander on 17-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMEPATTERNCASES_H
#define NPS_C_TRANSLATOR_LEXEMEPATTERNCASES_H

#include "../utils/string_utils.h"
#include "../heap/heap.h"

class LexemeCase
{
public:
    virtual ~LexemeCase(){};
    virtual bool IsMyChar(char ch) = 0;
    virtual int GetNextState() = 0;
};

class CharsRange
{
    char start, end;
public:
    CharsRange(char singleChar) {start = end = singleChar; }
    CharsRange(char _start, char _end) {start = _start; end = _end; }
    bool IsMyChar(char ch) { return ch >= start && ch <= end; }
};

class RangeCase : public LexemeCase
{
    CharsRange *range;
    int newState;
public:
    RangeCase(CharsRange *_range, int _newState)
    { range = _range; newState = _newState; }
    bool IsMyChar(char ch) override {return range->IsMyChar(ch);}
    int GetNextState() override {return newState;}
};

class StringCase : public LexemeCase
{
    char *str;
    int newState;
public:
    StringCase(const char *start, unsigned long length, int _newState)
    {str = copy_string(start, length); newState = _newState;}
    StringCase(const char *start, int _newState)
    {str = copy_string(start); newState = _newState;}
    
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

#endif //NPS_C_TRANSLATOR_LEXEMEPATTERNCASES_H
