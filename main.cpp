#include <iostream>
#include "LexemeParsing/LexemeParser.h"

using namespace std;

int main()
{
    LexemeParser p;
    LexemeStatesManager *statesManager = p.DELETETHISMETHODLATER();
    int i1 = statesManager->FindNextState(0, 'a');
    cout << "1: " << i1 << endl;
    i1 = statesManager->FindNextState(0, 'b');
    cout << "1: " << i1 << endl;
    i1 = statesManager->FindNextState(0, 'c');
    cout << "1: " << i1 << endl;
    i1 = statesManager->FindNextState(0, 'd');
    cout << "2: " << i1 << endl;
    i1 = statesManager->FindNextState(0, '1');
    cout << "0: " << i1 << endl;
    
    i1 = statesManager->FindNextState(1, 'c');
    cout << "0: " << i1 << endl;
    i1 = statesManager->FindNextState(1, 'C');
    cout << "3: " << i1 << endl;
    i1 = statesManager->FindNextState(1, '1');
    cout << "0: " << i1 << endl;
    i1 = statesManager->FindNextState(1, '0');
    cout << "3: " << i1 << endl;
    return 0;
}
