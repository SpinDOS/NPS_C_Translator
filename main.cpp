#include <iostream>
#include <fstream>
#include "LexemeParsing/LexemeParser.h"

void get_line_and_pos_of_char(const char *start, const char *position, int &line, int &pos);

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
    TypeList<LexemeError> errors;
    TypeList<LexemeInfo> *lexemes = ParseToLexemes(start, errors);
    if (errors.count())
    {
        for (int i = 0; i < errors.count(); i++)
        {
            LexemeError *error = errors.getTyped(0);
            int line, pos;
            get_line_and_pos_of_char(start, error->error_start, line, pos);
            cout << "Error: '" << error->message << "' in the lexeme '" << error->invalid_lexeme
                 << "' (line: " << line << ", position: " << pos << ")" << endl;
        }
    }
    else
    {
        cout << "Got lexemes: " << endl;
        for (int i = 0; i < lexemes->count(); i++)
            cout << lexemes->getTyped(i)->type_of_lexeme << ' ';
    }
    
    // free mem (unnecessary)
    for (int i = 0; i < lexemes->count(); i++)
        delete lexemes->getTyped(i)->description;
    delete lexemes;
    return 0;
}

void get_line_and_pos_of_char(const char *start, const char *position, int &line, int &pos)
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