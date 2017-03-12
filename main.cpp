#include <iostream>
#include <fstream>
#include <cstdlib>
#include "LexemeParsing/LexemeParser.h"

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos);
void printLexeme(LexemeInfo *lexemeInfo);

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file to parse>" << endl;
        return EXIT_FAILURE;
    }
    std::ifstream file(argv[1]);
    std::string contents((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    const char *start = contents.c_str();
    file.close();
    TypeList<LexemeError> errors;
    TypeList<LexemeInfo> *lexemes = ParseToLexemes(start, errors);
    if (errors.count())
    {
        for (int i = 0; i < errors.count(); i++)
        {
            LexemeError *error = errors.getTyped(i);
            int line, pos;
            getLinePosOfChar(start, error->error_start, line, pos);
            cout << "Error: '" << error->message << "' in the lexeme \"" << error->invalid_lexeme
                 << "\" (line: " << line << ", position: " << pos << ")" << endl;
        }
    }
    else
    {
        cout << "Got lexemes: " << endl;
        for (int i = 0; i < lexemes->count(); i++)
        {
            printLexeme(lexemes->getTyped(i));
        }
    }
    
    // free mem (unnecessary)
    for (int i = 0; i < lexemes->count(); i++)
        Heap::free_mem(lexemes->getTyped(i)->description);
    delete lexemes;
    return 0;
}

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos)
{
    line = 1;
    pos = 1;
    while (start < position)
    {
        if (*start++ == '\n')
        {
            line++;
            pos = 1;
        }
        else
            pos++;
    }
}

void printLexeme(LexemeInfo *lexemeInfo)
{
    unsigned char ch;
    LexemeType type = lexemeInfo->type_of_lexeme;
    switch (type)
    {
        case Keyword:
            cout << "Keyword: " << (char*) lexemeInfo->description;
            break;
        case VariableName:
            cout << "Variable: " << (char*) lexemeInfo->description;
            break;
        case Operation:
            cout << "Operation: code " << *((OperationToken*)lexemeInfo->description);
            break;
        case CharConstant:
            ch = *static_cast<unsigned char*>(lexemeInfo->description);
            cout << "Char constant: code: " << (int) ch << ", char: \'" << ch << "\'";
            break;
        case StringConstant:
            cout << "String constant: \"" << (char*) lexemeInfo->description << "\"";
            break;
        case NumConstant:
            NumConstantDescription *description = (NumConstantDescription*) lexemeInfo->description;
            cout << "Numeric constant: Type code: " << description->type << ", Value: " << description->num;
            break;
    }
    cout << endl;
}