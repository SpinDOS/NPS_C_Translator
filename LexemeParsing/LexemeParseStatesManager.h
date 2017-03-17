//
// Created by Alexander on 16-Mar-17.
//

#ifndef NPS_C_TRANSLATOR_LEXEMESTATESMANAGER_H
#define NPS_C_TRANSLATOR_LEXEMESTATESMANAGER_H

#include "../collection_containers/hash.h"
#include "LexemePatternCases.h"

class LexemeStatesManager : private Hash
{
public:
    LexemeStatesManager() : Hash(10, 10, 10, 10 , 0)
        { element_count = 10*10*10*10;}
    virtual ~LexemeStatesManager() override;
    int key1(const char *key_word) override
        { return (*reinterpret_cast<const int*>(key_word)/1000) % 10; };
    int key2(const char *key_word) override
        { return (*reinterpret_cast<const int*>(key_word)/100) % 10; };
    int key3(const char *key_word) override
        { return (*reinterpret_cast<const int*>(key_word)/10) % 10; };
    int key4(const char *key_word) override
        { return *reinterpret_cast<const int*>(key_word) % 10; };
    
    int FindNextState(int currentState, char currentChar);
    void Add(int oldState, LexemeCase *lexemeCase);
};

#endif //NPS_C_TRANSLATOR_LEXEMESTATESMANAGER_H
