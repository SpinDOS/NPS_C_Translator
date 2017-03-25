#include <iostream>
#include <fstream>
#include "LexemeParsing/LexemeParser.h"
#include "VariableParsing/VariableParser.h"

using namespace std;

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos);
void printLexeme(LexemeWord *lexemeInfo);
string parse_constant(LexemeWord *lexemeWord);

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
    const char *start = contents.c_str();
    if (!parser.ParseToLexemes(contents.c_str(), words, error))
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
    VariableParser variableParser;
    VariableError variableError;
    if(!variableParser.parse(words, variableError))
    {
        cout << "variable Error: " << variableError.name << " " << variableError.description << endl;
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
    int type = lexemeWord->code / 100;
    string str(lexemeWord->start, lexemeWord->length);
    switch (type)
    {
        case 1:
            cout << "Constant: ";
            str = parse_constant(lexemeWord);
            break;
        case 2:
            cout << "Operation: ";
            break;
        case 3:
            cout << "Keyword: ";
            break;
        case 4:
            cout << "Variable: ";
            break;
    }
    cout << str << " (code " << lexemeWord->code << ") " << endl;
}

string parse_constant(LexemeWord *lexemeWord)
{
    string result;
    LexemeError error;
    NumConstantType type = CharConstant;
    error.errorCode = -1;
    if (100 <= lexemeWord->code && lexemeWord->code < 110)
        result = "string: " + string(parse_string_constant(*lexemeWord));
    else if (110 <= lexemeWord->code && lexemeWord->code < 120)
        result = "char: " + string(1, parse_char_constant(*lexemeWord, error));
    else
    {
        result = to_string(parse_num_constant(*lexemeWord, type, error));
        result = "num (type " + to_string(type) + "): " + result;
    }
    if (error.errorCode == -1)
        return result;
    return "Error parsing constant " + string(lexemeWord->start, lexemeWord->length);
}