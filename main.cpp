#include <iostream>
#include <fstream>
#include "LexemeParsing/LexemeParser.h"

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos);
void printLexeme(LexemeWord *lexemeInfo);

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
    LexemeError error;
    TypeList<LexemeWord> words;
    LexemeParser parser;
    if (!parser.ParseToLexemes(contents.c_str(), words, &error))
    {
        int line, pos;
        char *invalidWord = copy_string(error.lexemeStart, error.errorStart - error.lexemeStart + 1);
        getLinePosOfChar(start, error.errorStart, line, pos);
        cout << "Error: '" << error.errorCode << "' in the lexeme \"" << invalidWord
             << "\" (line: " << line << ", position: " << pos << ")" << endl;
        Heap::free_mem(invalidWord);
    }
    else
    {
        cout << "Got lexemes: " << endl;
        for (int i = 0; i < words.count(); i++)
        {
            printLexeme(words.getTyped(i));
        }
    }
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

void printLexeme(LexemeWord *lexemeWord)
{
    int type = lexemeWord->code / 1000;
    switch (type)
    {
        case 2:
            cout << "Constant: '";
            break;
        case 3:
            cout << "Operation: '";
            break;
        case 4:
            cout << "Keyword: '";
            break;
        case 5:
            cout << "Variable: '";
            break;
    }
    string temp = lexemeWord->start;
    cout << temp.substr(0, lexemeWord->length)
         << "(code " << lexemeWord->code << ") " << endl;
}