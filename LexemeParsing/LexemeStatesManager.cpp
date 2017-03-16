//
// Created by Alexander on 16-Mar-17.
//

#include "LexemeStatesManager.h"

LexemeStatesManager::~LexemeStatesManager()
{
    for (int i = 0; i < 9999; i++)
    {
        TypeList<LexemeCase> *list = static_cast<TypeList<LexemeCase> *>
        (find_list((char *) &i));
        if (list == nullptr)
            continue;
        for (int j = 0; j < list->count(); j++)
        {
            LexemeCase *lexemeCase = list->getTyped(i);
            delete lexemeCase;
        }
    }
}

int LexemeStatesManager::FindNextState(int currentState, char currentChar)
{
    TypeList<LexemeCase> *list = static_cast<TypeList<LexemeCase> *>
    (find_list((char *) &currentState));
    for (int i = 0; i < list->count(); i++)
    {
        LexemeCase *lexemeCase = list->getTyped(i);
        if (lexemeCase->IsMyChar(currentChar))
            return lexemeCase->GetNextState();
    }
    return 0;
}

void LexemeStatesManager::Add(LexemeCase *lexemeCase, int state)
{
    List *list = find_list((char*)&state);
    if (list == nullptr)
        list = add_list((char*)&state, new TypeList<LexemeCase>);
    list->add(lexemeCase);
}
