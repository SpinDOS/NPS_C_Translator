#include <iostream>
#include <fstream>
#include "LexemeParsing/LexemeParser.h"

using namespace std;

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos);

int main(int argc, char *argv[])
{
    ifstream ini_file("LexemeParsingInstructions.ini");
    if (!ini_file.good())
    {
        cout << "LexemeParsingInstructions.ini not found" << endl;
        return EXIT_FAILURE;
    }
    string instructions((std::istreambuf_iterator<char>(ini_file)),
                         std::istreambuf_iterator<char>());
    ini_file.close();
    
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file to parse>" << endl;
        return EXIT_FAILURE;
    }
    ifstream file(argv[1]);
    if (!file.good())
    {
        cout << "Can not open file " << argv[1] << endl;
        return EXIT_FAILURE;
    }
    string contents((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    LexemeError error;
    TypeList<LexemeWord> words;
    LexemeParser parser(instructions.c_str());
    if (!parser.ParseToLexemes(contents.c_str(), words, error))
    {
        int line, pos;
        char *invalidWord = copy_string(error.lexemeStart, error.errorStart - error.lexemeStart + 1);
        getLinePosOfChar(contents.c_str(), error.errorStart, line, pos);
        cout << "Error: '" << error.errorCode << "' in the lexeme \"" << invalidWord
             << "\" (line: " << line << ", position: " << pos << ")" << endl;
        Heap::free_mem(invalidWord);
        return 1;
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