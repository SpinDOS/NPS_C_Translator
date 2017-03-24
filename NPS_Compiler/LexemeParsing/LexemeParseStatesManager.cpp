//
// Created by Alexander on 16-Mar-17.
//

#include "LexemeParseStatesManager.h"

LexemeStatesManager::~LexemeStatesManager()
{
    for (int i = 0; i < element_count; i++)
    {
        TypeList<LexemeCase*> *list = static_cast<TypeList<LexemeCase*> *>
            (find_list((char *) &i));
        if (list == nullptr)
            continue;
        for (int j = 0; j < list->count(); j++)
        {
            LexemeCase *lexemeCase = *list->getTyped(j);
            delete lexemeCase;
        }
    }
}

int LexemeStatesManager::FindNextState(int currentState, char currentChar)
{
    TypeList<LexemeCase*> *list = static_cast<TypeList<LexemeCase*> *>
        (find_list((char *) &currentState));
    if (list == nullptr)
        return 0;
    for (int i = 0; i < list->count(); i++)
    {
        LexemeCase *lexemeCase = *list->getTyped(i);
        if (lexemeCase->IsMyChar(currentChar))
            return lexemeCase->GetNextState();
    }
    return 0;
}

void LexemeStatesManager::Add(int oldState, LexemeCase *lexemeCase)
{
    char *charPointer = reinterpret_cast<char*>(&oldState);
    List *list = find_list(charPointer);
    if (list == nullptr)
        list = add_list(charPointer, new TypeList<LexemeCase*>);
    list->add(&lexemeCase);
}
